#pragma once
#include <memory>
#include <variant>
#include <vector>

/*
    Three-address-code (TAC) linear intermediate representation:

    An instruction is in the general form:
        Destination = Operand Operation Operand

    Where Destination can be one of:
        - Virtual Register

    And an Operand can be one of:
        - Virtual Register
        - Identifier (user-defined variable)
        - Integer / Floating point literal


    Each IR node represents a single TAC instruction
    An instruction can contain:
        - a maximum of one operation (e.g add, subtract)
        - a maximum of two operands

    Expression Examples:

    "x = a + 4" in TAC :
        t0 = a + 4      | Binary node
        a = t0          | assignment node

    "a = 1 + 2 + (-3)" in TAC :
        t0 = 1 + 2     | binary node
        t1 = -3        | unary node
        a = t0 + t1    | binary node

*/

namespace IR {

enum class OPERATION : std::uint8_t {
    ADD,
    SUB,
    MULT,
    DIV,
    CMPLMNT,
    NEG
};
enum class VREGTYPE : std::uint8_t { REGULAR, FLOATING_POINT };

// Forward declarations
struct BinaryNode;
struct UnaryNode;
struct AssignmentNode;
struct AssignToVreg;
struct ReturnNode;

// Syntactic sugar
using BinaryNodePtr = std::unique_ptr<BinaryNode>;
using UnaryNodePtr = std::unique_ptr<UnaryNode>;
using AssignmentNodePtr = std::unique_ptr<AssignmentNode>;
using ReturnNodePtr = std::unique_ptr<ReturnNode>;
using AssignToVregPtr = std::unique_ptr<AssignToVreg>;

using IRnode = std::variant<BinaryNodePtr,
                            UnaryNodePtr,
                            AssignmentNodePtr,
                            ReturnNodePtr,
                            AssignToVregPtr>;

// Virtual registers are compiler generated variables for holding the
// result of operations.
// RISC-V has a seperate register file for floating point operations
// so we need to encode which register is to be used in codegen
struct VirtualRegister {
    size_t ID;
    VREGTYPE Type;
};

// operands in TAC can either be:
// Virtual Register, variable or literal value (aka constant,
// immediate)
using Operand = std::variant<VirtualRegister, std::string, int>;

///////////////////////
// NODE DECLARATIONS //
///////////////////////

struct FunctionNode {
    std::string Name;
    std::vector<IRnode> Instructions;
};

// Result = Src1 Op Src2
struct BinaryNode {
    VirtualRegister Result;
    Operand Src1;
    OPERATION Op;
    Operand Src2;

    BinaryNode(VirtualRegister result,
               Operand src1,
               OPERATION op,
               Operand src2);
};

// Result = Op Src1
struct UnaryNode {
    VirtualRegister Result;
    OPERATION Op;
    Operand Src1;

    UnaryNode(VirtualRegister result, OPERATION op, Operand src1);
};

// VarName = Src1
struct AssignmentNode {
    std::string VarName;
    Operand Src1;

    AssignmentNode(std::string name, Operand src1);
};

struct AssignToVreg {
    VirtualRegister Result;
    std::string VarName;
};

struct ReturnNode {
    Operand ReturnVal;
    ReturnNode(Operand returnvalue);
};

} // namespace IR