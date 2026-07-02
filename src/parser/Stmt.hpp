#pragma once
#include "Expr.hpp"

#include <memory>
#include <variant>

// This file contains the declarations of AST nodes for Statements

// Forward declarations
struct IfStmt;
struct ReturnStmt;

// Syntatic sugar for node pointers
using IfStmtPtr = std::unique_ptr<IfStmt>;
using ReturnStmtPtr = std::unique_ptr<ReturnStmt>;

// Using variant + visit instead of dynamic polymorphism visitor pattern
using StmtNodePtr = std::variant<IfStmtPtr, ReturnStmtPtr>;

template <typename T>
concept StmtType = std::same_as<T, IfStmt> || std::same_as<T, ReturnStmt>;

template <StmtType T, typename... Args>
StmtNodePtr createStmtNode(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

struct ReturnStmt {
    ExprNodePtr m_RetValue;

    ReturnStmt(ExprNodePtr value);
};

struct IfStmt {
    ExprNodePtr m_Condition;
    StmtNodePtr m_Body;

    IfStmt(ExprNodePtr condition, StmtNodePtr body);
};
