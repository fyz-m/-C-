#pragma once
#include "Expr.hpp"

#include <memory>
#include <variant>
#include <vector>

// This file contains the declarations of AST nodes for Statements

// Forward declarations for 'using' statements
struct IfStmt;
struct ReturnStmt;
struct ExprStmt;
struct FunctionStmt;
struct VarDeclarationStmt;
struct BlockStmt;

// Syntatic sugar for node pointers
using IfStmtPtr = std::unique_ptr<IfStmt>;
using ReturnStmtPtr = std::unique_ptr<ReturnStmt>;
using ExprStmtPtr = std::unique_ptr<ExprStmt>;
using FunctionStmtPtr = std::unique_ptr<FunctionStmt>;
using VarDeclarationStmtPtr = std::unique_ptr<VarDeclarationStmt>;
using BlockStmtPtr = std::unique_ptr<BlockStmt>;

// Using variant + visit instead of dynamic polymorphism visitor pattern
using StmtNodePtr = std::variant<IfStmtPtr, ReturnStmtPtr, ExprStmtPtr, FunctionStmtPtr,
                                 VarDeclarationStmtPtr, BlockStmtPtr>;

///////////////////////
// NODE DECLARATIONS //
///////////////////////

struct ReturnStmt {
    std::optional<ExprNodePtr> m_RetValue = std::nullopt;

    ReturnStmt(std::optional<ExprNodePtr> value);
};

struct IfStmt {
    const ExprNodePtr m_Condition;
    const StmtNodePtr m_Body;
    // else body
    IfStmt(ExprNodePtr condition, StmtNodePtr body);
};

struct FunctionStmt {
    Token m_Identifier;
    BlockStmtPtr m_Body;

    FunctionStmt(Token identifier, BlockStmtPtr body);
};

struct VarDeclarationStmt {
    Token m_VarName;
    ExprNodePtr m_Initializer; // std::optional ?

    VarDeclarationStmt(Token name, ExprNodePtr initializer);
};

struct BlockStmt {
    std::vector<StmtNodePtr> m_statements;
    BlockStmt(std::vector<StmtNodePtr> statements);
};

struct ExprStmt {
    const ExprNodePtr m_Expr;

    ExprStmt(ExprNodePtr expr)
        : m_Expr{std::move(expr)} {}
};
