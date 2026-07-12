#include "IRgen.hpp"
/*
Three-address-code Intermediate Representation


stored as a "quadruple"
 - containing op
 - destination
 - src1
 - src2
*/

namespace IR {

size_t Generator::nextRegister() {
    return m_CurrRegister++;
}

void Generator::emit(OPCODE op, size_t src1, size_t src2, size_t dest) {
    m_QuadList.emplace_back(op, src1, src2, dest);
}

} // namespace IR
