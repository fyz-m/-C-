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
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");

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
/* 1 + 2 + 3
parseExpr1 called
parsePrefix returns literal<1>
minbp == 0
consume +
call parseinfix, passing in the current node literal<1>
parseinfix 1
    match on +
    call parseExpr (bp(+) + 1 )

parseExpr2
parsePrefix returns literal<2>
check bindingpower 10 >= 11 -> false
return literal <2>

parseinfix1 returns binary<1 + 2>

parseExpr1
now leftnode = binary<1 + 2>
loop again as bindingpower of + >= 0
call parseInifx passing in leftnode
    match +
    -> call parseExpr(11)
        -> return parsePrefix() -> literal<3>
    return new binary node ( + (+ 1 2) 3)

parseExpr1
leftnode = binary node ( + (+ 1 2) 3)
bindingpower() >= 0?  (bindingpower returns -1 for end of expr)
-> false -> return leftnode

*/
// Operator Precedence parsing (Pratt Parser)
ExprNodePtr Parser::parseExpr(int minBindingPower) {

    auto leftNode = parsePrefixExpr();

    while (getBindingPower(peek().type) >= minBindingPower) {
        leftNode = parseInfixExpr(std::move(leftNode));
    }
    return leftNode;
}

/*
Parsing and creating leaf expression:
- Literals e.g 7
- Unary    e.g -5)
- Grouping e.g '('expr')'
    - Included here so we
*/
ExprNodePtr Parser::parsePrefixExpr() {

    using enum TokenType;

    switch (peek().type) {

    case FLOAT_LITERAL:
    case INTEGER_LITERAL:
        return createAstNode<LiteralExpr>(std::move(advance().literal));

    case IDENTIFIER: {
        return createAstNode<IdentifierExpr>(std::move(advance()));
    }

    // Unary
    case TILDE:
    case MINUS_MINUS:
    case MINUS: {
        auto& op = advance();
        // Call parseExpr() with a very high binding power so it returns the next
        // prefix expression e.g literal(2).
        // Equivalent to calling parsePrefixExpr() recursively
        auto expr = parseExpr(UNARY_BP);
        return createAstNode<UnaryExpr>(std::move(op), std::move(expr));
    }

    // Grouping
    case LEFT_PAREN: {
        advance();
        auto expr = parseExpr();
        consume(RIGHT_PAREN, "Missing ')' after grouping expression");
        return expr;
    }

    default:
        reportError(peek(), "Expected expression");
    }
}

/*
Parsing and creating non-leaf expression nodes
We take the left subtree of a non-leaf expression
*/
ExprNodePtr Parser::parseInfixExpr(ExprNodePtr leftNode) {

    using enum TokenType;

    switch (peek().type) {

    // Binary
    case STAR:
    case SLASH:
    case MINUS:
    case PLUS: {
        auto& op = advance();
        // Add one to the binding power so expro of a chained operator is left associative
        // (e.g 1 + 2 + 3)
        // In the call to parseExpr(), the call will only evaluate a leaf expr node and return that
        auto rightExpr = parseExpr(getBindingPower(op.type) + 1);
        return createAstNode<BinaryExpr>(std::move(leftNode), std::move(op), std::move(rightExpr));
    }
    // Assignment
    case EQUAL: {
        if (auto* identifierNode = std::get_if<IdentifierExprPtr>(&leftNode)) {
            advance();
            auto rightExpr = parseExpr(getBindingPower(EQUAL));
            return createAstNode<AssignmentExpr>(std::move(*identifierNode), std::move(rightExpr));
        }
        reportError(previous(), "Invalid assignment target (can only assign to variables)");
    }

    case IDENTIFIER:
    case TILDE:
    case MINUS_MINUS:
    case LEFT_PAREN:
    case FLOAT_LITERAL:
    case INTEGER_LITERAL:
        reportError(peek(), "Malformed expression (missing operator)");

    default:
        reportError(peek(), "Malformed infix expression");
    }
}

int Parser::getBindingPower(TokenType _operator) {
    auto it = m_BindingPower.find(_operator);

    // Return -1 for any token that isn't a valid operator in an expression
    // to indicate the expression has finished.
    // minBindingPower will always be >= 0, so returning -1 will cause the
    // whileloop condition in parseExpr to evaluate to false, returning the
    // expression subtree it has generated
    if (it == m_BindingPower.end())
        return -1;
    return it->second;
};

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

Token& Parser::peekNext() {
    return m_Tokens[m_Current + 1];
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