#pragma once
#include "parser/Ast/Stmt.hpp"

namespace IR {

class Generator;

// Visitor for emitting quadruples for expressions
struct EmitExpr {

    Generator& gen;
    EmitExpr(Generator& gen)
        : gen{gen} {}

    size_t operator()(const LiteralExprPtr& expr) const;
    size_t operator()(const BinaryExprPtr& expr) const;
    size_t operator()(const UnaryExprPtr& expr) const;
    size_t operator()(const IdentifierExprPtr& expr) const;
    size_t operator()(const AssignmentExprPtr& expr) const;
};

// Visitor for emitting quadruples for statements
struct EmitStmt {

    Generator& gen;
    EmitStmt(Generator& gen)
        : gen{gen} {}

    size_t operator()(const ExprStmtPtr& stmt) const;
    size_t operator()(const BlockStmtPtr& stmt) const;
    size_t operator()(const ReturnStmtPtr& stmt) const;
    size_t operator()(const IfStmtPtr& stmt) const;
    size_t operator()(const FunctionStmtPtr& stmt) const;
    size_t operator()(const VarDeclarationStmtPtr& stmt) const;
};

} // namespace IR