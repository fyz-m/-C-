#pragma once
#include "../IR/IRnodes.hpp"
#include "ASM/CreateASMnode.hpp"
#include "ASM/RISC-V.hpp"

#include <vector>

namespace CODEGEN {

template <class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

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
    // Convert an IR operand into a RISC-V operand. Because a
    // RISC-V operand cannot be an integer (immediates are stored as
    // int, not in the operand variant).
    // An IR operand that is an integer must be handled in the
    // IRvisitor before casting dowm the variant.
    static RISCV::Operand castVariant(IR::Operand& operand) {

        auto op = RISCV::Operand{};

        std::visit(
            Overloaded{
                [](int) {
                    // Should be unreachable as
                    // function should never be called
                    // if operand contains <int> (enforced in
                    // IRvisitor control flow)
                    throw std::runtime_error(
                        "Attempt to convert integer into "
                        "RISC-V operand");
                },

                [&](IR::VirtualRegister) {
                    op = RISCV::Operand{
                        std::get<IR::VirtualRegister>(operand)};
                },

                [&](std::string&) {
                    op = RISCV::Operand{
                        std::get<std::string>(operand)};
                },
            },
            operand);

        return op;
    }

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
