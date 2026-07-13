#include "IRgen.hpp"

namespace IR {

// dst = LI imm
size_t Generator::loadIntToReg(int integer) {
    return 0;
}

size_t Generator::nextRegister() {
    return m_CurrRegister++;
}

} // namespace IR
