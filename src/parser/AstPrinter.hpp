#pragma once
#include "Ast/Expr.hpp"
#include "Ast/Stmt.hpp"

#include <vector>

namespace AST {

class Printer {

  public:
    static void printAST(const std::vector<StmtNodePtr>& nodes);
    static std::string printExpr(const ExprNodePtr& exprNode);
    static std::string printStmt(const StmtNodePtr& stmtNode);
};

// PRINTING EXPRESSIONS
struct PrintExpr {
    std::string operator()(const LiteralExprPtr& expr) const;
    std::string operator()(const BinaryExprPtr& expr) const;
    std::string operator()(const UnaryExprPtr& expr) const;
    std::string operator()(const IdentifierExprPtr& expr) const;

    std::string parenthesize(const std::string& name,
                             std::initializer_list<const ExprNodePtr*> exprNodes) const;
};

// PRINTING STATEMENTS
struct PrintStmt {
    std::string operator()(const ExprStmtPtr& stmt) const;
    std::string operator()(const ReturnStmtPtr& stmt) const;
    std::string operator()(const IfStmtPtr& stmt) const;
    std::string operator()(const FunctionStmtPtr& stmt) const;

    std::string indent(const std::string& name,
                       std::initializer_list<const StmtNodePtr*> stmtNodes) const;
};

} // namespace AST