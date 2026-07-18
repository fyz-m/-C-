#include "IRgen.hpp"

namespace IR {

Generator::Generator(std::span<const StmtNodePtr> ast)
    : m_AST{ast} {}

std::vector<IRnode>& Generator::generateIR() {
    for (const auto& node : m_AST)
        std::visit(StmtVisitor{*this}, node);

    return m_Nodes;
}

void StmtVisitor::operator()(const ExprStmtPtr& stmt) const {
    std::visit(ExprVisitor{this->Gen}, stmt->m_Expr);
}

void StmtVisitor::operator()(const BlockStmtPtr& stmt) const {}

void StmtVisitor::operator()(const ReturnStmtPtr& stmt) const {
    auto retval = std::visit(ExprVisitor{this->Gen}, stmt->m_RetValue.value());
    Gen.emit<IR::ReturnNode>(std::move(retval));
}

void StmtVisitor::operator()(const IfStmtPtr& stmt) const {}

void StmtVisitor::operator()(const FunctionStmtPtr& stmt) const {}

void StmtVisitor::operator()(const VarDeclarationStmtPtr& stmt) const {}

Operand ExprVisitor::operator()(const LiteralExprPtr& expr) const {
    if (auto* p = std::get_if<int>(&expr->m_Value))
        return *p;

    return "null";
}

Operand ExprVisitor::operator()(const BinaryExprPtr& expr) const {

    auto src1 = std::visit(*this, expr->m_leftNode);
    auto src2 = std::visit(*this, expr->m_rightNode);
    auto result = Gen.getTempVar();

    Gen.emit<IR::BinaryNode>(result,
                             std::move(src1),
                             Generator::getBinaryOp(expr->m_operation.type),
                             std::move(src2));
    return result;
}

Operand ExprVisitor::operator()(const UnaryExprPtr& expr) const {

    auto src1 = std::visit(*this, expr->m_expression);
    auto result = Gen.getTempVar();

    Gen.emit<IR::UnaryNode>(
        result, Generator::getUnaryOp(expr->m_operation.type), std::move(src1));
    return result;
}

Operand ExprVisitor::operator()(const IdentifierExprPtr& expr) const {
    auto result = Gen.getTempVar();
    Gen.emit<IR::AssignToVreg>(result, expr->m_Name.lexeme);
    return result;
}

Operand ExprVisitor::operator()(const AssignmentExprPtr& expr) const {
    auto src1 = std::visit(*this, expr->m_Value);
    std::string name = expr->m_Identifier->m_Name.lexeme;

    Gen.emit<IR::AssignmentNode>(expr->m_Identifier->m_Name.lexeme,
                                 std::move(src1));
    return name;
}

Variable Generator::getTempVar(bool floating_point) {
    // '.' is not allowed as an identifier
    // We use it for compiler generated variables so there is no collision with
    // user-defined variables
    if (floating_point)
        return std::format("tfp.{}", m_CurrRegisterFP++);

    return std::format("t.{}", m_CurrRegister++);
}

} // namespace IR
