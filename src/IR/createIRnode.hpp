#pragma once

#include "IRnodes.hpp"

template <typename T>
concept IRnodeType = std::same_as<T, IR::BinaryNode> || std::same_as<T, IR::UnaryNode>;

template <IRnodeType T, typename... Args>
auto createIRnode(Args... args) {
    auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
    return IR::IRnode{std::move(ptr)};
}