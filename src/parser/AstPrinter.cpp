#include "AstPrinter.hpp"

#include <print>
#include <sstream>

namespace AST {

void Printer::printAST(const std::vector<StmtNodePtr>& nodes) {
    for (const auto& node : nodes)
        std::println("{}", Printer::printStmt(node));
}

std::string Printer::printStmt(const StmtNodePtr& stmtNode) {
    return std::visit(PrintStmt{}, stmtNode);
}

std::string Printer::printExpr(const ExprNodePtr& exprNode) {
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

std::string PrintExpr::operator()(const UnaryExprPtr& expr) const {
    return parenthesize(expr->m_operation.lexeme, {&expr->m_expression});
}

std::string PrintExpr::operator()(const IdentifierExprPtr& expr) const {
    return expr->m_Name.lexeme;
}

// (name expr...)
std::string
PrintExpr::parenthesize(const std::string& name,
                        const std::initializer_list<const ExprNodePtr*> exprNodes) const {

    std::stringstream s;
    s << '(' << name;
    for (const auto& expr : exprNodes) {
        s << " ";
        s << std::visit(PrintExpr{}, *expr);
    }
    s << ')';
    return s.str();
}

std::string PrintStmt::operator()(const ExprStmtPtr& stmt) const {
    return Printer::printExpr(stmt->m_Expr);
}

std::string PrintStmt::operator()(const ReturnStmtPtr& stmt) const {
    if (stmt->m_RetValue.has_value())
        return std::format("RETURN: {}", Printer::printExpr(stmt->m_RetValue.value()));
    return std::format("RETURN (nothing)");
}

std::string PrintStmt::operator()(const IfStmtPtr& stmt) const {
    return std::format("IF\n\tCONDITION:{}\n{}", Printer::printExpr(stmt->m_Condition),
                       indent("BODY:", {&stmt->m_Body}));
}

std::string PrintStmt::operator()(const FunctionStmtPtr& stmt) const {
    return std::format("FUNCTION\n  NAME = \"{}\"\n  {}", stmt->m_Identifier.lexeme,
                       indent("BODY", {&stmt->m_Body}));
}

std::string PrintStmt::indent(const std::string& name,
                              std::initializer_list<const StmtNodePtr*> stmtNodes) const {
    std::stringstream s;
    s << name;
    for (const auto* stmt : stmtNodes) {
        s << "\n    | ";
        s << std::visit(PrintStmt{}, *stmt);
    }
    s << '\n';
    return s.str();
}

} // namespace AST