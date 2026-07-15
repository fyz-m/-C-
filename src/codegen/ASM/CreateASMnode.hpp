#include "asm.hpp"

namespace ASM {

template <typename T>
concept RV_INSTRUCTION = std::same_as<T, ADD> || std::same_as<T, ADDI> ||
                         std::same_as<T, RET> || std::same_as<T, MV>;

template <RV_INSTRUCTION T, typename... Args>
auto createASMnode(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

} // namespace ASM