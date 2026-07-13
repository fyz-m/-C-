#pragma once

#include "IRnodes.hpp"

template <typename T>
concept IRnode = std::same_as<T, IR::BinaryNode> || std::same_as<T, IR::UnaryNode>;

template <IRnode T, typename... Args>
auto createIRnode(Args... args) {
    auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
    return IR::IRnode{std::move(ptr)};
}