#pragma once
#include "RISC-V.hpp"

namespace RISCV {

// Pseudo instructions
template <typename T>
concept RV_PSINSTR =
    std::same_as<T, RISCV::RET> || std::same_as<T, RISCV::MV> ||
    std::same_as<T, RISCV::NOT> || std::same_as<T, RISCV::NEG> ||
    std::same_as<T, RISCV::LI>;

template <typename T>
concept RV_INSTR = std::same_as<T, RISCV::Rtype> ||
                   std::same_as<T, RISCV::Itype> || RV_PSINSTR<T>;

template <RV_INSTR T, typename... Args>
[[nodiscard]]
auto createInstruction(Args... args) {
    return RISCV::Instruction{
        std::make_unique<T>(std::forward<Args>(args)...)};
}

} // namespace RISCV