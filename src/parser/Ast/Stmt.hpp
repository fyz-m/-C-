#pragma once
#include "Expr.hpp"

#include <memory>
#include <variant>

// This file contains the declarations of AST nodes for Statements

// Forward declarations for 'using' statements
struct IfStmt;
struct ReturnStmt;
struct ExprStmt;
struct FunctionStmt;

// Syntatic sugar for node pointers
using IfStmtPtr = std::unique_ptr<IfStmt>;
using ReturnStmtPtr = std::unique_ptr<ReturnStmt>;
using ExprStmtPtr = std::unique_ptr<ExprStmt>;
using FunctionStmtPtr = std::unique_ptr<FunctionStmt>;

// Using variant + visit instead of dynamic polymorphism visitor pattern
using StmtNodePtr = std::variant<IfStmtPtr, ReturnStmtPtr, ExprStmtPtr, FunctionStmtPtr>;

template <typename T>
concept StmtType = std::same_as<T, IfStmt> || std::same_as<T, ReturnStmt> ||
                   std::same_as<T, ExprStmt> || std::same_as<T, FunctionStmt>;

template <StmtType T, typename... Args>
StmtNodePtr createAstNode(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

///////////////////////
// NODE DECLARATIONS //
///////////////////////

struct ReturnStmt {
    const ExprNodePtr m_RetValue;

    ReturnStmt(ExprNodePtr value);
};

struct IfStmt {
    const ExprNodePtr m_Condition;
    const StmtNodePtr m_Body;
    // else body
    IfStmt(ExprNodePtr condition, StmtNodePtr body);
};

struct FunctionStmt {
    Token m_Identifier;
    StmtNodePtr m_Body;

    FunctionStmt(Token identifier, StmtNodePtr body);
};

struct ExprStmt {
    const ExprNodePtr m_Expr;

    ExprStmt(ExprNodePtr expr)
        : m_Expr{std::move(expr)} {}
};
