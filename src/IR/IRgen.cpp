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
    auto retval =
        std::visit(ExprVisitor{this->Gen}, stmt->m_RetValue.value());
    Gen.emit<IR::ReturnNode>(std::move(retval));
}

void StmtVisitor::operator()(const IfStmtPtr& stmt) const {}

void StmtVisitor::operator()(const FunctionStmtPtr& stmt) const {}

void StmtVisitor::operator()(
    const VarDeclarationStmtPtr& stmt) const {}

Operand ExprVisitor::operator()(const LiteralExprPtr& expr) const {
    if (auto* p = std::get_if<int>(&expr->m_Value))
        return *p;

    return "null";
}

Operand ExprVisitor::operator()(const BinaryExprPtr& expr) const {

    auto src1 = std::visit(ExprVisitor{Gen}, expr->m_leftNode);
    auto src2 = std::visit(ExprVisitor{Gen}, expr->m_rightNode);

    // if caller was an AssignmentExpr visitor, the result of this
    // expression is stored in the variable that the expression is
    // being assigned to.
    Variable result;
    if ((*this).Resultname.has_value())
        result = Resultname.value();
    // else create new variable and store result in that
    else
        result = Gen.getTempVar();

    Gen.emit<IR::BinaryNode>(
        result,
        std::move(src1),
        Generator::getBinaryOp(expr->m_operation.type),
        std::move(src2));

    return result;
}

Operand ExprVisitor::operator()(const UnaryExprPtr& expr) const {

    auto src1 = std::visit(ExprVisitor{Gen}, expr->m_expression);

    Variable result;

    if ((*this).Resultname.has_value())
        // Need to cast the std::optional for move semantics
        result = std::move(Resultname).value();
    else
        result = Gen.getTempVar();

    Gen.emit<IR::UnaryNode>(
        result,
        Generator::getUnaryOp(expr->m_operation.type),
        std::move(src1));
    return result;
}

Operand ExprVisitor::operator()(const IdentifierExprPtr& expr) const {
    return expr->m_Name.lexeme;
}

Operand ExprVisitor::operator()(const AssignmentExprPtr& expr) const {

    std::string var_name =
        std::move(expr->m_Identifier->m_Name.lexeme);

    auto src1 = std::visit(ExprVisitor{Gen, var_name}, expr->m_Value);

    // If the exprNode (Value) that the AssignmentNode held did not
    // nest deeper (i.e contained another exprNode), the result of
    // converting that expr into 3AC (src1) would land itself to only
    // one 3AC instruction.
    // Meaning src1 == var_name,
    auto* res = std::get_if<std::string>(&src1);
    if (res && *res == var_name)
        return var_name;

    auto var_name_cpy = var_name;
    Gen.emit<IR::AssignmentNode>(std::move(var_name),
                                 std::move(src1));
    return var_name_cpy;
}

Variable Generator::getTempVar(bool floating_point) {
    // '.' is not allowed as an identifier
    // We use it for compiler generated variables so there is no
    // collision with user-defined variables
    if (floating_point)
        return std::format("tfp.{}", m_CurrRegisterFP++);

    return std::format("t.{}", m_CurrRegister++);
}

} // namespace IR
