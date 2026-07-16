#include "IRnodes.hpp"

#include <utility>

namespace IR {
BinaryNode::BinaryNode(VirtualRegister result,
                       Operand src1,
                       OPERATION op,
                       Operand src2)
    : Result{result}, Src1{std::move(src1)}, Op{op}, Src2{std::move(src2)} {}

UnaryNode::UnaryNode(VirtualRegister result, OPERATION op, Operand src1)
    : Result{result}, Op{op}, Src1{std::move(src1)} {}

AssignmentNode::AssignmentNode(std::string name, Operand src1)
    : VarName{std::move(name)}, Src1{std::move(src1)} {}

ReturnNode::ReturnNode(Operand returnvalue)
    : ReturnVal{std::move(returnvalue)} {}

} // namespace IR