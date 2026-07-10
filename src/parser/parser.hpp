#pragma once

#include "../lexer/Token.hpp"
#include "Ast/Expr.hpp"
#include "Ast/Stmt.hpp"

#include <expected>
#include <unordered_map>
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

    // Parsing statements

    std::optional<StmtNodePtr> parseDeclaration();

    StmtNodePtr parseFunctionDecl();

    StmtNodePtr parseVariableDecl();

    StmtNodePtr parseStmt();

    StmtNodePtr parseReturnStmt();

    std::vector<StmtNodePtr> parseBlock();

    StmtNodePtr parseExprStmt();

    // Parsing expressions

    ExprNodePtr parseExpr(int minBindingPower = 0);

    ExprNodePtr parsePrefixExpr();

    ExprNodePtr parseInfixExpr(ExprNodePtr leftNode);

    // Helpers
  private:
    // Conditional advance/consume
    bool match(TokenType expected);

    bool match(std::initializer_list<TokenType>& exepectedTypes);

    bool check(TokenType expected);

    Token& consume(TokenType expected, const std::string& error);

    // Return current token and consume it
    Token& advance();

    // Return previous token
    Token& previous();

    // Return current token without consuming
    Token& peek();

    Token& peekNext();

    bool isatEnd();

    // Reports error to diag engine and
    // sets parser into to error recovery
    // by throwing ParseError()
    void reportError(Token& token, std::string&& errorMessage);

    void synchronize();

    int getBindingPower(TokenType _operator);

    std::unordered_map<TokenType, int> m_BindingPower{
        {TokenType::EQUAL, 5}, {TokenType::PLUS, 10},  {TokenType::MINUS, 10},
        {TokenType::STAR, 20}, {TokenType::SLASH, 20},
    };
};
constexpr int UNARY_BP = 100;

struct ParseError : public std::exception {};