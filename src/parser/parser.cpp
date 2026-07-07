#include "parser.hpp"

#include "../diagnostics/DiagnosticsEngine.hpp"

Statements Parser::Parse() {

    std::vector<StmtNodePtr> statements;
    statements.reserve(100);

    while (!isatEnd())
        if (auto result = parseDeclaration())
            statements.push_back(std::move(result.value()));

    return statements;
}

std::optional<StmtNodePtr> Parser::parseDeclaration() {
    using enum TokenType;
    try {

        if (match(DEF))
            return parseFunctionDecl();
        if (match(INT) || match(FLOAT))
            return parseVariableDecl();

        return parseStmt();

    } catch (const ParseError&) {
        synchronize();
        return std::nullopt;
    }
}

StmtNodePtr Parser::parseFunctionDecl() {
    Token& name = consume(TokenType::IDENTIFIER, "Expect identifier after function declaration.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after function identifier");
    consume(TokenType::VOID, "Expected 'void'");
    consume(TokenType::RIGHT_PAREN, "Expect ')'");
    consume(TokenType::LEFT_BRACE, "Expect '{'.");

    auto body = std::make_unique<BlockStmt>(parseBlock());

    return createAstNode<FunctionStmt>(std::move(name), std::move(body));
}

StmtNodePtr Parser::parseVariableDecl() {
    Token& name = consume(TokenType::IDENTIFIER, "Expected identifier after variable declaration");
    consume(TokenType::EQUAL, "Expected '=' to intialize variable");
    auto initializer = parseExpr();
    consume(TokenType::SEMICOLON, "Expected ';' after variable initializer");

    return createAstNode<VarDeclarationStmt>(std::move(name), std::move(initializer));
}

StmtNodePtr Parser::parseStmt() {

    // if (match(TokenType::IF))
    //     return parseIfStmt();
    if (match(TokenType::RETURN))
        return parseReturnStmt();
    if (match(TokenType::LEFT_BRACE))
        return createAstNode<BlockStmt>(parseBlock());

    return parseExprStmt();
}

StmtNodePtr Parser::parseReturnStmt() {
    std::optional<ExprNodePtr> expr;
    if (!check(TokenType::SEMICOLON))
        expr = parseExpr();
    consume(TokenType::SEMICOLON, "Expected ';' after return expression.");

    return createAstNode<ReturnStmt>(std::move(expr));
}

std::vector<StmtNodePtr> Parser::parseBlock() {

    std::vector<StmtNodePtr> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isatEnd())
        if (auto decl = parseDeclaration())
            statements.push_back(std::move(decl.value()));

    consume(TokenType::RIGHT_BRACE, "Missing closing '}'");
    return statements;
}

StmtNodePtr Parser::parseExprStmt() {
    auto expr = parseExpr();
    consume(TokenType::SEMICOLON, "Expected expression");

    return createAstNode<ExprStmt>(std::move(expr));
}

// Operator Precedence parsing (Pratt Parser)
ExprNodePtr Parser::parseExpr(size_t min_bp) {

    auto leftNode = parsePrefixExpr();

    while (true) {
        auto& op = peek();

        // Two identifiers or literals in a row
        // E.g "1 + 2 3"
        if (op.type == TokenType::IDENTIFIER || op.literal.index() != 0)
            reportError(peek(), "Malformed expression");

        auto it = m_bindingPower.find(op.type);

        // If next token is not an infix operator,
        // finish parsing and return the expression tree
        if (it == m_bindingPower.end())
            break;

        if (it->second.lbp < min_bp)
            break;

        advance();
        auto rightNode = parseExpr(it->second.rbp); //
        leftNode =
            createAstNode<BinaryExpr>(std::move(leftNode), std::move(op), std::move(rightNode));
    }
    return leftNode;
}

ExprNodePtr Parser::parsePrefixExpr() {

    using enum TokenType;

    switch (peek().type) {

    case FLOAT_LITERAL:
    case INTEGER_LITERAL:
        return createAstNode<LiteralExpr>(std::move(advance().literal));

    case IDENTIFIER:
        return createAstNode<IdentifierExpr>(std::move(advance()));

    // Unary
    case TILDE:
    case MINUS_MINUS:
    case MINUS: {
        auto& op = advance();
        auto expr = parseExpr(unaryBindingPower());
        return createAstNode<UnaryExpr>(std::move(op), std::move(expr));
    }

    case LEFT_PAREN: {
        advance();
        auto expr = parseExpr();
        consume(RIGHT_PAREN, "Missing ')' after grouping expression");
        return expr;
    }

    default:
        reportError(peek(), "Malformed or missing expression");
    }
}

size_t Parser::unaryBindingPower() const {
    return 100;
}

bool Parser::match(TokenType expected) {

    if (peek().type == expected) {
        ++m_Current;
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType>& exepectedTypes) {

    for (const auto& expectedType : exepectedTypes) {
        if (peek().type == expectedType) {
            ++m_Current;
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType expected) {
    return peek().type == expected;
}

Token& Parser::consume(TokenType expected, const std::string& error) {
    if (peek().type == expected)
        return m_Tokens[m_Current++];

    std::string s =
        check(TokenType::END_OF_FILE) ? error : error + ", found '" + peek().lexeme + "' instead";

    reportError(peek(), std::move(s));
    return peek();
}

Token& Parser::advance() {
    if (!isatEnd())
        ++m_Current;
    return previous();
}

Token& Parser::previous() {
    return m_Tokens[m_Current - 1];
}

Token& Parser::peek() {
    return m_Tokens[m_Current];
}

bool Parser::isatEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

void Parser::reportError(Token& token, std::string&& errorMessage) {

    auto* err_tok = &token;
    if (err_tok->type == TokenType::END_OF_FILE)
        // Ignore EOF token because diag engine will attempt
        // to read past source string (UB)
        err_tok = &previous();

    Diagnostics::DiagnosticsEngine::report(*err_tok, std::move(errorMessage));
    throw ParseError();
}

void Parser::synchronize() {
    advance();

    using enum TokenType;

    // Keep consuming tokens until keyword or ;
    while (!isatEnd()) {
        if (previous().type == SEMICOLON)
            return;

        switch (peek().type) {
        case IF:
        case ELSE:
        case WHILE:
        case FOR:
        case RETURN:
        case BREAK:
        case VOID:
        case INT:
        case FLOAT:
            return;
        default:
            advance();
        }
    }
}