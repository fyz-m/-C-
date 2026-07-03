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
        // consume identifier
        // If match '(' function decl
        // else variable decl
    }

    return parseStmt();
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

ExprNodePtr Parser::parseExpr() {}

void Parser::consume(TokenType expected, std::string_view errorMessage) {
    if (peek().type == expected) {
        ++m_Current;
        return;
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

const Token& Parser::peekNext() const {
    return m_Tokens[m_Current + 1];
}

bool Parser::isatEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}