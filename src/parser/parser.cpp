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

    try {

        if (match(TokenType::INT)) {

            return parseFunctionDecl();
        }

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

    auto body = parseStmt();
    consume(TokenType::RIGHT_BRACE, "Expect '}' After function body.");

    return createAstNode<FunctionStmt>(std::move(name), std::move(body));
}

StmtNodePtr Parser::parseStmt() {

    // if (match(TokenType::IF))
    //     return parseIfStmt();
    if (match(TokenType::RETURN))
        return parseReturnStmt();

    return parseExprStmt();
}

StmtNodePtr Parser::parseReturnStmt() {
    auto expr = parseExpr();
    consume(TokenType::SEMICOLON, "Expected ';' after return expression.");

    return createAstNode<ReturnStmt>(std::move(expr));
}

StmtNodePtr Parser::parseExprStmt() {
    auto expr = parseExpr();
    consume(TokenType::SEMICOLON, "Expected ';' after expression.");

    return createAstNode<ExprStmt>(std::move(expr));
}

// Operator Precedence parsing
ExprNodePtr Parser::parseExpr(size_t min_bp) {

    /*
        ExprNode leftNode  = parsePrefix()  // first thing in expression, e.g literal, unary)

        while true {

            op = peek().type            // Look at operator after prefix
            if (op != infix_operator)   // End of expression, break and return
                break;
            if (op.bp < min_bp)                // If operators binding power is >= min_bp,
                break;                  // take the prefix and recurse right to form a node

            advance()       // consume operator
            rightNode = parseExpression(op.bp)  //  Parse right side
            leftNode = (leftNode, op, rightNode)
        }
        return leftNode
    */

    auto leftNode = parsePrefixExpr();

    while (true) {
        auto& op = peek();
        auto it = m_bindingPower.find(op.type);
        if (it == m_bindingPower.end())
            break;

        auto op_bp = it->second;
        if (op_bp < min_bp)
            break;

        advance();
        auto rightNode = parseExpr(op_bp + 1);
        leftNode =
            createAstNode<BinaryExpr>(std::move(leftNode), std::move(op), std::move(rightNode));
    }

    return leftNode;
}

ExprNodePtr Parser::parsePrefixExpr() {

    ExprNodePtr node;
    using enum TokenType;

    switch (peek().type) {

    case FLOAT_LITERAL:
    case INTEGER_LITERAL:
        node = createAstNode<LiteralExpr>(peek().literal);
        break;

        /*
            case UnaryExpr (e.g '-'):
                node = create<unary>(
                    op = peek()
                    literal = parsePrefixExpr()
                )
        */

    default:
        Diagnostics::DiagnosticsEngine::report(peek(), "Malformed or missing expression");
        throw ParseError();
    }

    advance();
    return node;
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

Token& Parser::consume(TokenType expected, std::string_view errorMessage) {
    if (peek().type == expected)
        return m_Tokens[m_Current++];

    Diagnostics::DiagnosticsEngine::report(peek(), errorMessage);
    throw ParseError();
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