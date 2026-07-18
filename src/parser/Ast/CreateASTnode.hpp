#pragma once
#include "Stmt.hpp"

#include <cassert>
#include <concepts>

// Helper method to create an Expr/Stmt node as a variant
// so I don't need to make a new function for every node
// E.g createBinaryExpr, createIdentifierExpr etc.

template <typename T>
concept ExprNode =
    std::same_as<T, BinaryExpr> || std::same_as<T, UnaryExpr> || std::same_as<T, LiteralExpr> ||
    std::same_as<T, IdentifierExpr> || std::same_as<T, AssignmentExpr>;

template <typename T>
concept StmtNode = std::same_as<T, IfStmt> || std::same_as<T, ReturnStmt> ||
                   std::same_as<T, ExprStmt> || std::same_as<T, FunctionStmt> ||
                   std::same_as<T, VarDeclarationStmt> || std::same_as<T, BlockStmt>;

template <typename T>
concept ASTNode = ExprNode<T> || StmtNode<T>;

template <ASTNode T, typename... Args>
auto createAstNode(Args... args) {

    auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

    if constexpr (ExprNode<T>)
        return ExprNodePtr{std::move(ptr)};
    if constexpr (StmtNode<T>)
        return StmtNodePtr{std::move(ptr)};
}