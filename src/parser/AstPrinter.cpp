#include "AstPrinter.hpp"

#include "Ast/Expr.hpp"
#include "sstream"

std::string Print(const ExprNodePtr& exprNode) {
    return std::visit(PrintExpr{}, exprNode);
}

std::string PrintExpr::operator()(const LiteralExprPtr& expr) const {

    if (const auto* p = std::get_if<int>(&expr->m_Value)) {
        return std::to_string(*p);
    }
    if (expr->m_Value.index() == 0) {
        return "None";
    }
    if (const auto* p = std::get_if<float>(&expr->m_Value)) {
        return std::to_string(*p);
    }
    return std::get<std::string>(expr->m_Value);
}

std::string PrintExpr::operator()(const BinaryExprPtr& expr) const {
    return parenthesize(expr->m_operation.lexeme, {&expr->m_leftNode, &expr->m_rightNode});
}

std::string parenthesize(std::string_view name,
                         const std::initializer_list<const ExprNodePtr*> exprNodes) {

    std::stringstream s;
    s << '(' << name;
    for (const auto& expr : exprNodes) {
        s << " ";
        s << std::visit(PrintExpr{}, *expr);
    }
    s << " )";
    return s.str();
}
