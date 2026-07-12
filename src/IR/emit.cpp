#include "emit.hpp"

#include "IRgen.hpp"

namespace IR {

size_t EmitStmt::operator()(const ExprStmtPtr& stmt) const {
    return 0;
}

size_t EmitStmt::operator()(const BlockStmtPtr& stmt) const {
    return 0;
}

size_t EmitStmt::operator()(const ReturnStmtPtr& stmt) const {
    if (!stmt->m_RetValue) {}
    auto val = gen.emitExpr(stmt->m_RetValue.value());
    gen.emit(OPCODE::RET, val);
    return 0;
}

size_t EmitStmt::operator()(const IfStmtPtr& stmt) const {
    return 0;
}

size_t EmitStmt::operator()(const FunctionStmtPtr& stmt) const {
    return 0;
}

size_t EmitStmt::operator()(const VarDeclarationStmtPtr& stmt) const {
    return 0;
}

size_t EmitExpr::operator()(const LiteralExprPtr& expr) const {
    if (const auto* p = std::get_if<int>(&expr->m_Value)) {
        return gen.loadIntToReg(*p);
    }
    return 0;
}
// dst = src1 op src2
size_t EmitExpr::operator()(const BinaryExprPtr& expr) const {
    auto src1 = gen.emitExpr(expr->m_leftNode);
    auto src2 = gen.emitExpr(expr->m_leftNode);
    return gen.emit(gen.nextRegister(), gen.getOpcodeBinary(expr->m_operation.type), src1, src2);
}

// dst = op src1
size_t EmitExpr::operator()(const UnaryExprPtr& expr) const {
    // evaluate expression and store it in src1
    auto src = std::visit(*this, expr->m_expression);
    return gen.emit(gen.nextRegister(), gen.getOpcodeUnary(expr->m_operation.type), src);
}

size_t EmitExpr::operator()(const IdentifierExprPtr& expr) const {
    return 0;
}

size_t EmitExpr::operator()(const AssignmentExprPtr& expr) const {
    return 0;
}

} // namespace IR