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

    And an Operand can be one of:
        - Virtual Register
        - Identifier (user-defined variable)
        - Literal/Immediate/Constant (e.g like the value '4')

    Note 'Destination' cannot be a literal value, therefore I've
    called the variant for holding the result of an operation
    'Assignable'.

    Each IR node represents a single TAC instruction
    An instruction can contain:
        - a maximum of one operation (e.g add, subtract)
        - a maximum of two operands

    Expression Examples:

    "x = a + 4" in TAC :
        x = a + 4     | binary node

    "a = 1 + 2 + (-3)" in TAC :
        t0 = 1 + 2    | binary node
        t1 = -3       | unary node
        a = t0 + t1   | binary node

    "x = a + 4" in TAC :
        x = a + 4

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

// RISC-V has a seperate register file for floating point operations
// so we need to encode which register is to be used in codegen
struct VirtualRegister {
    size_t ID;
    VREGTYPE Type;
};

// operands in TAC can either be:
// Identifier (user-defined variable), Vreg, or literal value (aka constant, immediate)
using Operand = std::variant<std::string, VirtualRegister, int>;

// The result of an operation can only be assigned to an identifier or Vreg
using Assignable = std::variant<std::string, VirtualRegister>;

// Dst = Src1 Op Src2
struct BinaryNode {
    Assignable Dst;
    Operand Src1;
    OPERATION Op;
    Operand Src2;

    BinaryNode(Assignable dst, Operand src1, OPERATION op, Operand src2);
};

// Dst = Op Src1
struct UnaryNode {
    Assignable Dst;
    Operand Src1;
    OPERATION Op;

    UnaryNode(Assignable dst, OPERATION op, Operand src1);
};

} // namespace IR