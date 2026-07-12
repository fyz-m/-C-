#include "IRgen.hpp"
/*
Three-address-code Intermediate Representation


stored as a "quadruple"
 - containing op
 - destination
 - src1
 - src2

 We need functions that emit commonly used operations
 such as loading a literal
*/

namespace IR {

size_t Generator::emitExpr(const ExprNodePtr& expr) {
    return std::visit(EmitExpr{*this}, expr);
}

size_t Generator::emitStmt(const StmtNodePtr& stmt) {
    return std::visit(EmitStmt{*this}, stmt);
}

// dst = LI imm
size_t Generator::loadIntToReg(int integer) {
    return emit(nextRegister(), OPCODE::LI, integer);
}

size_t Generator::emit(size_t dest, OPCODE op, std::optional<size_t> src1,
                       std::optional<size_t> src2) {
    m_QuadList.emplace_back(dest, op, src1, src2);
    return dest;
}

void Generator::emit(OPCODE op, std::optional<size_t> src1, std::optional<size_t> src2) {
    m_QuadList.emplace_back(std::nullopt, op, src1, src2);
}

size_t Generator::nextRegister() {
    return m_CurrRegister++;
}

} // namespace IR
