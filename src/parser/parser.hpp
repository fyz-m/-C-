#pragma once

#include "../lexer/Token.hpp"
#include "Ast/Expr.hpp"
#include "Ast/Stmt.hpp"

#include <expected>
#include <vector>

using Statements = std::expected<std::vector<StmtNodePtr>, bool>;
class Parser {

  private:
    std::vector<Token> m_Tokens;
    size_t m_Current{0};

  public:
    Parser(std::vector<Token>& tokens)
        : m_Tokens{std::move(tokens)} {}

    Statements Parse();

  private:
    // Parsing statements

    std::optional<StmtNodePtr> parseDeclaration();

    StmtNodePtr parseFunctionDecl();

    // StmtNodePtr parseVariableDecl();

    StmtNodePtr parseStmt();

    StmtNodePtr parseReturnStmt();

    // StmtNodePtr parseIfStmt();

    StmtNodePtr parseExprStmt();

    // Parsing expressions

    ExprNodePtr parseExpr();

    // Helpers

    // Conditional advance/consume
    bool match(TokenType expected);

    bool match(std::initializer_list<TokenType>& exepectedTypes);

    bool check(TokenType expected);

    Token& consume(TokenType expected, std::string_view errorMessage);

    // Return current token and consume it
    Token& advance();

    // Return previous token
    Token& previous();

    // Return current token without consuming
    Token& peek();

    Token& peekNext();

    bool isatEnd();

    void synchronize();
};

struct ParseError : public std::exception {
    Token m_Token;
    ParseError(Token& token)
        : m_Token(token) {}
};