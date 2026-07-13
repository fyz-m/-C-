#include "IRgen.hpp"

namespace IR {

VirtualRegister Generator::loadIntToReg(int integer) {}

VirtualRegister Generator::getRegister() {
    return {.ID = m_CurrRegister++, .Type = VREGTYPE::REGULAR};
}

VirtualRegister Generator::getRegisterFP() {
    return {.ID = m_CurrRegisterFP++, .Type = VREGTYPE::FLOATING_POINT};
}

} // namespace IR
