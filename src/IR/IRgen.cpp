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

void StmtVisitor::operator()(const ReturnStmtPtr& stmt) const {}

void StmtVisitor::operator()(const IfStmtPtr& stmt) const {}

void StmtVisitor::operator()(const FunctionStmtPtr& stmt) const {}

void StmtVisitor::operator()(const VarDeclarationStmtPtr& stmt) const {}

Operand ExprVisitor::operator()(const LiteralExprPtr& expr) const {
    if (auto* p = std::get_if<int>(&expr->m_Value))
        return *p;

    return VirtualRegister{};
}

Operand ExprVisitor::operator()(const BinaryExprPtr& expr) const {

    auto src1 = std::visit(*this, expr->m_leftNode);
    auto src2 = std::visit(*this, expr->m_rightNode);
    auto result = Gen.getRegister();

    Gen.emit<IR::BinaryNode>(result,
                             std::move(src1),
                             Generator::getBinaryOp(expr->m_operation.type),
                             std::move(src2));
    return result;
}

Operand ExprVisitor::operator()(const UnaryExprPtr& expr) const {

    auto src1 = std::visit(*this, expr->m_expression);
    auto result = Gen.getRegister();

    Gen.emit<IR::UnaryNode>(
        result, Generator::getUnaryOp(expr->m_operation.type), std::move(src1));
    return result;
}

Operand ExprVisitor::operator()(const IdentifierExprPtr& expr) const {
    return expr->m_Name.lexeme;
}

Operand ExprVisitor::operator()(const AssignmentExprPtr& expr) const {
    auto src1 = std::visit(*this, expr->m_Value);

    Gen.emit<IR::AssignmentNode>(expr->m_Identifier->m_Name.lexeme,
                                 std::move(src1));
    return 0;
}

VirtualRegister Generator::loadIntToReg(int integer) {
    auto reg = getRegister();
    // emit<AssignToVregNode>(reg, integer);
    return reg;
}

VirtualRegister Generator::getRegister(VREGTYPE type) {
    using enum VREGTYPE;
    if (type == REGULAR)
        return {.ID = m_CurrRegister++, .Type = REGULAR};
    return {.ID = m_CurrRegisterFP++, .Type = FLOATING_POINT};
}

} // namespace IR
