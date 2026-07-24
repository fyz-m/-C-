#pragma once
#include "IR/IRnodes.hpp"

#include <memory>

/*

Data structure to represent RISC-V assembly.

RISC-V instructions are highly regular, each node represents an
instruction type. Each type has the same format:

R-type: op rd, rs1, rs1

Pseudoinstructions exist for convenience, they are converted to real
instructions later.

*/

namespace RISCV {

using i32 = int32_t;

// RISC-V registers, numeric and names
enum class REGISTER : std::uint8_t {
    x0 = 0,
    zero = 0,

    x1 = 1,
    ra = 1,

    x2 = 2,
    sp = 2,

    x3 = 3,
    gp = 3,

    x4 = 4,
    tp = 4,

    x5 = 5,
    t0 = 5,

    x6 = 6,
    t1 = 6,

    x7 = 7,
    t2 = 7,

    x8 = 8,
    s0 = 8,
    fp = 8,

    x9 = 9,
    s1 = 9,

    x10 = 10,
    a0 = 10,

    x11 = 11,
    a1 = 11,

    x12 = 12,
    a2 = 12,

    x13 = 13,
    a3 = 13,

    x14 = 14,
    a4 = 14,

    x15 = 15,
    a5 = 15,

    x16 = 16,
    a6 = 16,

    x17 = 17,
    a7 = 17,

    x18 = 18,
    s2 = 18,

    x19 = 19,
    s3 = 19,

    x20 = 20,
    s4 = 20,

    x21 = 21,
    s5 = 21,

    x22 = 22,
    s6 = 22,

    x23 = 23,
    s7 = 23,

    x24 = 24,
    s8 = 24,

    x25 = 25,
    s9 = 25,

    x26 = 26,
    s10 = 26,

    x27 = 27,
    s11 = 27,

    x28 = 28,
    t3 = 28,

    x29 = 29,
    t4 = 29,

    x30 = 30,
    t5 = 30,

    x31 = 31,
    t6 = 31
};

namespace OPCODE {
enum class R_TYPE : std::uint8_t { add, sub };
enum class I_TYPE : std::uint8_t { addi, subi, xori, jalr };
enum class J_TYPE : std::uint8_t { jal };
enum class S_TYPE : std::uint8_t { beq };
}; // namespace OPCODE

/////////////////////////
// Pseudo-instructions //
/////////////////////////

struct RET;
struct MV;
struct LI;
struct NOT;
struct NEG;

using RetPtr = std::unique_ptr<RET>;
using MvPtr = std::unique_ptr<MV>;
using LIPtr = std::unique_ptr<LI>;
using NotPtr = std::unique_ptr<NOT>;
using NegPtr = std::unique_ptr<NEG>;

using PseudoInstrution =
    std::variant<RetPtr, MvPtr, NotPtr, NegPtr, LIPtr>;

struct Stack {
    int Offset;
};

// std::string == User-defined or compiler generated temporary
// Stack == variable stored on the stack
// Register == Actual RISC-V register

// We progressively lower the Operand variant through a series of
// passes until we only have registers.
using Operand = std::variant<IR::Variable, Stack, REGISTER>;

// return from function
struct RET {};

// Move/copy register
struct MV {
    Operand Rd;
    Operand Rs1;
};

// Load immediate
struct LI {
    Operand Rd;
    i32 Imm;
};

struct NEG {
    Operand Rd;
    Operand Rs1;
};

struct NOT {
    Operand Rd;
    Operand Rs1;
};

/////////////////////////
// RISC-V Instructions //
/////////////////////////

struct Rtype;
struct Itype;
struct InstructionList;

using RtypePtr = std::unique_ptr<Rtype>;
using ItypePtr = std::unique_ptr<Itype>;
using InstructionListPtr = std::unique_ptr<InstructionList>;

using Instruction = std::
    variant<RtypePtr, ItypePtr, InstructionListPtr, PseudoInstrution>;

struct Rtype {
    OPCODE::R_TYPE Op;
    Operand Rd;
    Operand Rs1;
    Operand Rs2;
};

struct Itype {
    OPCODE::I_TYPE Op;
    Operand Rd;
    Operand Rs1;
    i32 Imm;
};

// Later passes in the compiler like register allocation and
// pseudo-instruction translation usually replace one instruction with
// 2 or more, this struct is used by those passes instead of inserting
// in the Instructions vector (outputted by codegen) which would be
// very slow since vector will be resizing and shifting elements
// frequently
struct InstructionList {
    std::vector<Instruction> Instructions;
};
} // namespace RISCV