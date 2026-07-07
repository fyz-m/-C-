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
    std::stringstream s;
    m_indentLevel += indentIncrementLevel();
    s << printIndent() << "ExprStmt:\n";
    s << indentAndPrint("└─" + Printer::printExpr(stmt->m_Expr), -2);
    m_indentLevel -= indentIncrementLevel();

    return s.str();
}

std::string PrintStmt::operator()(const BlockStmtPtr& stmt) const {
    std::stringstream s;
    m_indentLevel += indentIncrementLevel();
    s << printIndent() << "BlockStmt:\n";
    for (auto& stmtNode : stmt->m_statements) {
        s << std::visit(*this, stmtNode);
    }
    m_indentLevel -= indentIncrementLevel();
    return s.str();
}

std::string PrintStmt::operator()(const VarDeclarationStmtPtr& stmt) const {
    std::stringstream s;
    // print type alsp
    m_indentLevel += indentIncrementLevel();
    s << printIndent() << "VarDeclStmt:\n";
    s << indentAndPrint("├──Identifier \'" + stmt->m_VarName.lexeme + "\'", -2);
    s << indentAndPrint("└──init \'" + Printer::printExpr(stmt->m_Initializer) + "\'", -2);
    m_indentLevel -= indentIncrementLevel();

    return s.str();
}

std::string PrintStmt::operator()(const ReturnStmtPtr& stmt) const {
    std::stringstream s;
    std::string return_value =
        stmt->m_RetValue.has_value() ? Printer::printExpr(stmt->m_RetValue.value()) : "none";

    m_indentLevel += indentIncrementLevel();
    s << printIndent() << "ReturnStmt\n";
    s << indentAndPrint("└─ret " + return_value, -2);

    m_indentLevel -= indentIncrementLevel();
    return s.str();
}

std::string PrintStmt::operator()(const IfStmtPtr& stmt) const {}

std::string PrintStmt::operator()(const FunctionStmtPtr& stmt) const {
    PrintStmt p = *this;
    std::stringstream s;
    m_indentLevel += indentIncrementLevel();
    s << "FunctionStmt: " << stmt->m_Identifier.lexeme << "\n";
    s << p(stmt->m_Body);
    m_indentLevel -= indentIncrementLevel();
    return s.str();
}

std::string PrintStmt::printIndent(int offset) const {
    return std::string(m_indentLevel + offset, ' ');
}

size_t PrintStmt::indentIncrementLevel() const {
    return 2;
}

std::string PrintStmt::indentAndPrint(const std::string& str, int offset) const {
    m_indentLevel += indentIncrementLevel();
    auto s = std::format("{}{}\n", printIndent(offset), str);
    m_indentLevel -= indentIncrementLevel();
    return s;
}

} // namespace AST