#pragma once
#include "../lexer/Token.hpp"

#include <memory>
#include <variant>

// This file contains the declarations of AST nodes for Expressions

// Forward declarations for syntatic sugar
struct BinaryExpr;
struct LiteralExpr;

// Syntatic sugar for node pointers
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using LiteralExprPtr = std::unique_ptr<LiteralExpr>;

// Using variant + visit instead of dynamic polymorphism (visitor pattern)
using ExprNodePtr = std::variant<BinaryExprPtr, LiteralExprPtr>;

///////////////////////
// NODE DECLARATIONS //
///////////////////////

struct BinaryExpr {
    const ExprNodePtr m_leftNode;
    const Token m_operation;
    const ExprNodePtr m_rightNode;

    BinaryExpr(ExprNodePtr left, Token& operation, ExprNodePtr right);
};

struct LiteralExpr {
    const LiteralValue m_Value;

    LiteralExpr(LiteralValue& value);
};

// Helper method to create an Expression node as a variant
// Using template so I don't need to make a new function for every expression node
// E.g createBinaryExpr, createLiteralExpr etc.
template <typename T>
concept ExprType = std::same_as<T, BinaryExpr> || std::same_as<T, LiteralExpr>;

template <ExprType T, typename... Args>
ExprNodePtr createNode(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}