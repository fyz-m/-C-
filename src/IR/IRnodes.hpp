#pragma once

#include <memory>
#include <variant>

/*
    Three-address-code (TAC) linear intermediate representation:

    An instruction is in the general form:
        Destination = Operand Operation Operand

    Where Destination can be one of:
        - Virtual Register
        - Identifier (user-defined variable)
            - Identifiers can only be assigned a Virtual register

    And an Operand can be one of:
        - Virtual Register
        - Literal/Immediate/Constant (e.g like the value '4')

    Identifiers must be assigned a virtual register before being
    used in operations.

    Each IR node represents a single TAC instruction
    An instruction can contain:
        - a maximum of one operation (e.g add, subtract)
        - a maximum of two operands

    Expression Examples:

    "x = a + 4" in TAC :
        t0 = a         | Load var (into vreg) node
        t1 = t0 + 4    | Binary node
        x = t1         | store (vreg into var) node

    "a = 1 + 2 + (-3)" in TAC :
        t0 = 1 + 2     | binary node
        t1 = -3        | unary node
        t2 = t0 + t1   | binary node
        a = t2         | store node

*/

namespace IR {

enum class OPERATION : std::uint8_t { ADD, SUB, MULT, DIV, CMPLMNT };
enum class VREGTYPE : std::uint8_t { REGULAR, FLOATING_POINT };

// Forward declarations
struct BinaryNode;
struct UnaryNode;

// Syntactic sugar
using BinaryNodePtr = std::unique_ptr<BinaryNode>;
using UnaryNodePtr = std::unique_ptr<UnaryNode>;

using IRnode = std::variant<BinaryNodePtr, UnaryNodePtr>;

// Virtual registers are compiler generated variables for holding the
// result of operations.
// RISC-V has a seperate register file for floating point operations
// so we need to encode which register is to be used in codegen
struct VirtualRegister {
    size_t ID;
    VREGTYPE Type;
};

// operands in TAC can either be:
// Virtual Register, or literal value (aka constant, immediate)
using Operand = std::variant<VirtualRegister, int>;

///////////////////////
// NODE DECLARATIONS //
///////////////////////

// Dst = Src1 Op Src2
struct BinaryNode {
    VirtualRegister Dst;
    Operand Src1;
    OPERATION Op;
    Operand Src2;

    BinaryNode(VirtualRegister dst, Operand src1, OPERATION op, Operand src2);
};

// Dst = Op Src1
struct UnaryNode {
    VirtualRegister Dst;
    Operand Src1;
    OPERATION Op;

    UnaryNode(VirtualRegister dst, OPERATION op, Operand src1);
};

} // namespace IR