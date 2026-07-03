
#include "Ast/Expr.hpp"

struct PrintExpr {
    std::string operator()(const LiteralExprPtr& expr) const;

    std::string operator()(const BinaryExprPtr& expr) const;

    std::string operator()(const IdentifierExprPtr& expr) const;

    std::string parenthesize(std::string_view name,
                             std::initializer_list<const ExprNodePtr*> exprNodes) const;
};

std::string Print(ExprNodePtr exprNode);