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

// Syntatic sugar for node pointers
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using LiteralExprPtr = std::unique_ptr<LiteralExpr>;
using IdentifierExprPtr = std::unique_ptr<IdentifierExpr>;

// Using variant + visit instead of dynamic polymorphism (visitor pattern)
using ExprNodePtr = std::variant<BinaryExprPtr, UnaryExprPtr, LiteralExprPtr, IdentifierExprPtr>;

// Helper method to create an Expression node as a variant
// Using template so I don't need to make a new function for every expression node
// E.g createBinaryExpr, createIdentifierExpr etc.
template <typename T>
concept ExprType = std::same_as<T, BinaryExpr> || std::same_as<T, UnaryExpr> ||
                   std::same_as<T, LiteralExpr> || std::same_as<T, IdentifierExpr>;

template <ExprType T, typename... Args>
ExprNodePtr createAstNode(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

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
