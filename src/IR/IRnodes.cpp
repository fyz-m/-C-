#include "IRnodes.hpp"

#include <utility>

namespace IR {
BinaryNode::BinaryNode(Assignable result,
                       Operand src1,
                       OPERATION op,
                       Operand src2)
    : Result{std::move(result)},
      Src1{std::move(src1)},
      Op{op},
      Src2{std::move(src2)} {}

UnaryNode::UnaryNode(Assignable result, OPERATION op, Operand src1)
    : Result{std::move(result)}, Op{op}, Src1{std::move(src1)} {}

AssignmentNode::AssignmentNode(Assignable name, Operand src1)
    : VarName{std::move(name)}, Src1{std::move(src1)} {}
} // namespace IR