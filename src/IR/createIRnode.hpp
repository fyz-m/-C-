#pragma once

#include "IRnodes.hpp"

template <typename T>
concept IRnodeType = std::same_as<T, IR::BinaryNode> ||
                     std::same_as<T, IR::UnaryNode> ||
                     std::same_as<T, IR::AssignmentNode> ||
                     std::same_as<T, IR::ReturnNode> ||
                     std::same_as<T, IR::AssignToVreg>;

template <IRnodeType T, typename... Args>
auto createIRnode(Args... args) {
    auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
    return IR::IRnode{std::move(ptr)};
}