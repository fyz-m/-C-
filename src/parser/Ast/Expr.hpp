#pragma once
#include "../../lexer/Token.hpp"

#include <memory>
#include <variant>

// This file contains the declarations of AST nodes for Expressions

// Forward declarations for syntatic sugar
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct IdentifierExpr;
struct AssignmentExpr;

// Syntatic sugar for node pointers
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using LiteralExprPtr = std::unique_ptr<LiteralExpr>;
using IdentifierExprPtr = std::unique_ptr<IdentifierExpr>;
using AssignmentExprPtr = std::unique_ptr<AssignmentExpr>;

// Using variant + visit instead of dynamic polymorphism (visitor pattern)
using ExprNodePtr =
    std::variant<BinaryExprPtr, UnaryExprPtr, LiteralExprPtr, IdentifierExprPtr, AssignmentExprPtr>;

///////////////////////
// NODE DECLARATIONS //
///////////////////////

struct BinaryExpr {
    const ExprNodePtr m_leftNode;
    const Token m_operation; // Do we need to store the entire token? TokenType Enum is what we need
                             // to determine op
    const ExprNodePtr m_rightNode;

    BinaryExpr(ExprNodePtr left, Token operation, ExprNodePtr right);
};

struct UnaryExpr {
    Token m_operation;
    ExprNodePtr m_expression;

    UnaryExpr(Token operation, ExprNodePtr expression);
};

struct LiteralExpr {
    LiteralValue m_Value;

    LiteralExpr(LiteralValue value);
};

struct IdentifierExpr {
    Token m_Name;
    IdentifierExpr(Token name);
};

struct AssignmentExpr {
    IdentifierExprPtr m_Identifier;
    ExprNodePtr m_Value;
    AssignmentExpr(IdentifierExprPtr identifier, ExprNodePtr value);
};