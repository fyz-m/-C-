#include "IRnodes.hpp"

#include <utility>

namespace IR {
BinaryNode::BinaryNode(VirtualRegister result, Operand src1, OPERATION op, Operand src2)
    : Result{result}, Src1{src1}, Op{op}, Src2{src2} {}

UnaryNode::UnaryNode(VirtualRegister result, OPERATION op, Operand src1)
    : Result{result}, Op{op}, Src1{src1} {}

AssignToVregNode::AssignToVregNode(VirtualRegister result, std::string src1)
    : Result{result}, Src1{std::move(src1)} {}
} // namespace IR