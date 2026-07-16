#include "RISC-V.hpp"

namespace ASM {

template <typename T>
concept RV_INSTRUCTION =
    std::same_as<T, RISCV::Rtype> || std::same_as<T, RISCV::Itype>;

template <RV_INSTRUCTION T, typename... Args>
auto createAsmInstruction(Args... args) {
    return RISCV::Instruction{
        std::make_unique<T>(std::forward<Args>(args)...)};
}

} // namespace ASM