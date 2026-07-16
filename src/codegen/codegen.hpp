#pragma once
#include "../IR/IRnodes.hpp"
#include "ASM/CreateASMnode.hpp"
#include "ASM/RISC-V.hpp"

#include <vector>

namespace CODEGEN {

// The code generator maps each TAC IR construct to a construct in a
// lower level IR that resembles RISC-V assembly. It determines the
// the required RISC-V instructions that are equivalent to the IR
// node.
//
// Pseudo instructions are expanded by a later pass
//
// Coverting variables to real RISC-V registers is done by the
// register allocator.

class CodeGenerator {

  private:
    std::span<IR::IRnode> m_IRnodes;
    std::vector<RISCV::Instruction> m_Instructions;

  public:
    CodeGenerator(std::span<IR::IRnode> IRnodes)
        : m_IRnodes(IRnodes) {
        // Each IR node will approximately be equivalent to 2 RISC-V
        // instructions, so reserve to minimize resizing
        // of vector.
        m_Instructions.reserve(IRnodes.size() * 2);
    }

    std::vector<RISCV::Instruction>& generateRISCVassembly();

  private:
    // Wrapper that pushes the instruction onto the vector
    template <RISCV::RV_INSTR T, typename... Args>
    void pushInstruction(Args... args) {
        m_Instructions.push_back(
            RISCV::createInstruction<T>(std::forward<Args>(args)...));
    }

    // IR node visitor
    // Logic to convert each IR node into assembly construct
    struct IRvisitor {
        CodeGenerator& CG;

        IRvisitor(CodeGenerator& cg)
            : CG{cg} {}

        void operator()(const IR::BinaryNodePtr& node);
        void operator()(const IR::UnaryNodePtr& node);
        void operator()(const IR::ReturnNodePtr& node);
        void operator()(const IR::AssignmentNodePtr& node);
        void operator()(const IR::AssignToVregPtr& node);
    };
};

} // namespace CODEGEN
