#pragma once
#include "Ast/Expr.hpp"
#include "Ast/Stmt.hpp"

#include <utility>
#include <vector>

// Print out an Abstract-Syntax tree using post-order traversal (DFS)

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
  private:
    mutable size_t m_indentLevel{};

    size_t indentIncrementLevel() const;

  public:
    std::string operator()(const ExprStmtPtr& stmt) const;
    std::string operator()(const BlockStmtPtr& stmt) const;
    std::string operator()(const ReturnStmtPtr& stmt) const;
    std::string operator()(const IfStmtPtr& stmt) const;
    std::string operator()(const FunctionStmtPtr& stmt) const;
    std::string operator()(const VarDeclarationStmtPtr& stmt) const;

    std::string printIndent(int offset = 0) const;
    std::string indentAndPrint(const std::string& str, int offset = 0) const;
};

} // namespace AST