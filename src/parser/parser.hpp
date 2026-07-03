#pragma once

#include "../lexer/Token.hpp"
#include "Ast/Expr.hpp"
#include "Ast/Stmt.hpp"

#include <vector>

class Parser {

  private:
    std::vector<Token> m_Tokens;
    size_t m_Current{0};

  public:
    Parser(std::vector<Token>& tokens)
        : m_Tokens{std::move(tokens)} {}

    std::vector<StmtNodePtr> Parse();

  private:
    // Parsing statements

    StmtNodePtr parseDeclaration();

    StmtNodePtr parseFunctionDecl();

    StmtNodePtr parseVariableDecl();

    StmtNodePtr parseStmt();

    StmtNodePtr parseReturnStmt();

    IfStmtPtr parseIfStmt();

    StmtNodePtr parseExprStmt();

    // Parsing expressions

    ExprNodePtr parseExpr();

    // Helpers

    // Conditional advance/consume
    bool match(TokenType expected);

    bool match(std::initializer_list<Token>& exepectedTypes);

    bool check(TokenType expected) const;

    Token& consume(TokenType expected, std::string_view errorMessage);

    // Return current token and consume it
    Token& advance();

    // Return previous token
    Token& previous();

    // Return current token without consuming
    const Token& peek() const;

    const Token& peekNext() const;

    bool isatEnd() const;

    void synchronize();
};

struct ParseError : public std::runtime_error {
    ParseError();
};