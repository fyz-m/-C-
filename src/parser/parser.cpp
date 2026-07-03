#include "parser.hpp"

std::vector<StmtNodePtr> Parser::Parse() {

    std::vector<StmtNodePtr> statements;
    statements.reserve(100);

    while (!isatEnd())
        statements.push_back(parseDeclaration());

    return statements;
}

StmtNodePtr Parser::parseDeclaration() {

    if (match(TokenType::INT)) {

        return parseFunctionDecl();
    }

    return parseStmt();
}

StmtNodePtr Parser::parseFunctionDecl() {
    Token& name = consume(TokenType::IDENTIFIER, "Expect identifier after function declaration.");
    consume(TokenType::LEFT_PAREN, "Expect '('");
    consume(TokenType::VOID, "Expect 'void'");
    consume(TokenType::RIGHT_PAREN, "Expect ')'");
    consume(TokenType::LEFT_BRACE, "Expect '{'.");

    auto body = parseStmt();
    consume(TokenType::RIGHT_BRACE, "Expect '}' After function body.");

    return createAstNode<FunctionStmt>(std::move(name), std::move(body));
}

StmtNodePtr Parser::parseStmt() {

    if (match(TokenType::IF))
        return parseIfStmt();
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
ExprNodePtr Parser::parseExpr() {
    if (match(TokenType::INTEGER_LITERAL))
        return createAstNode<LiteralExpr>(previous().literal);
}

Token& Parser::consume(TokenType expected, std::string_view errorMessage) {
    if (peek().type == expected) {
        ++m_Current;
        return previous();
    }
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

const Token& Parser::peek() const {
    return m_Tokens[m_Current];
}

bool Parser::isatEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}