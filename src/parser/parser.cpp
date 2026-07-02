#include "parser.hpp"

std::vector<StmtNodePtr> Parser::Parse() {

    std::vector<StmtNodePtr> statements;
    statements.reserve(100);

    try {
        while (!isatEnd())
            statements.push_back(parseStmt());
    } catch (const std::runtime_error& e) {
        synchronize();
    }

    return statements;
}

StmtNodePtr Parser::parseStmt() {
    if (match(TokenType::IF))
        return parseIfStmt();
    if (match(TokenType::RETURN))
        return parseReturnStmt();

    return parseExprStmt();
}

StmtNodePtr Parser::parseExprStmt() {
    auto expr = parseExpr();
    // If no ';' syntax error

    return createAstNode<ExprStmt>(std::move(expr));
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