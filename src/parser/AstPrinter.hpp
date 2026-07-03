#pragma once
#include "Ast/Expr.hpp"
#include "Ast/Stmt.hpp"

class Printer {

  public:
    static std::string printExpr(const ExprNodePtr& exprNode);
    static std::string printStmt(const StmtNodePtr& stmtNode);
};

// PRINTING EXPRESSIONS
struct PrintExpr {
    std::string operator()(const LiteralExprPtr& expr) const;
    std::string operator()(const BinaryExprPtr& expr) const;
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

void PrintAST(const ExprNodePtr& exprNode);