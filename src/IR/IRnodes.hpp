#pragma once
#include <memory>
#include <variant>
#include <vector>

/*
    Three-address-code (TAC) linear intermediate representation:

    An instruction is in the general form:
        Destination = Operand Operation Operand

    Where Destination can be one of:
        - Variable

    And an Operand can be one of:
        - Variable
        - Integer / Floating point literal

    Variables can be user-defined or compiler generated
    The compiler creates variables to store intermediate results of
    expressions with more than two operands (See examples below).

   Each IR node represents a
   single TAC instruction An instruction can contain:
        - a maximum of one operation (e.g add, subtract)
        - a maximum of two operands

    Expression Examples:

    "x = a + 4" in TAC :
        x = a + 4      | Binary node

    "a = 1 + 2 + (-3)" in TAC :
        t.0 = 1 + 2     | binary node
        t.1 = -3        | unary node
        a = t.0 + t.1    | binary node

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

// operands in TAC can either be:
// variable (user-defined or compiler generated) or literal value (aka
// constant, immediate)
using Operand = std::variant<std::string, int>;
using Variable = std::string;

///////////////////////
// NODE DECLARATIONS //
///////////////////////

struct FunctionNode {
    std::string Name;
    std::vector<IRnode> Instructions;
};

// Result = Src1 Op Src2
struct BinaryNode {
    Variable Result;
    Operand Src1;
    OPERATION Op;
    Operand Src2;

    BinaryNode(Variable result,
               Operand src1,
               OPERATION op,
               Operand src2);
};

// Result = Op Src1
struct UnaryNode {
    Variable Result;
    OPERATION Op;
    Operand Src1;

    UnaryNode(Variable result, OPERATION op, Operand src1);
};

// VarName = Src1
struct AssignmentNode {
    std::string VarName;
    Operand Src1;

    AssignmentNode(std::string name, Operand src1);
};

struct AssignToVreg {
    Variable Result;
    std::string VarName;
};

struct ReturnNode {
    // std::optional<operand>
    Operand ReturnVal;
    ReturnNode(Operand returnvalue);
};

} // namespace IR