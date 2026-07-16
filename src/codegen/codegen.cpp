#include "codegen.hpp"

#include "ASM/CreateASMnode.hpp"

using namespace RISCV;
using namespace IR;
using RISCV::OPCODE;
using enum RISCV::OPCODE::I_TYPE;
using enum REGISTER;

template <class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

namespace CODEGEN {

std::vector<RISCV::Instruction>&
CodeGenerator::generateRISCVassembly() {
    for (const auto& IRnode : m_IRnodes)
        std::visit(IRvisitor{*this}, IRnode);
    return m_Instructions;
}

void CodeGenerator::IRvisitor::operator()(const BinaryNodePtr& node) {
}

void CodeGenerator::IRvisitor::operator()(const UnaryNodePtr& node) {}

void CodeGenerator::IRvisitor::operator()(const ReturnNodePtr& node) {

    // Place return value in a0 register
    // a0 is the return value register in RISC-V convention
    std::visit(
        Overloaded{
            // Add immediate to a0 if returning a literal integer
            [&](int) {
                CG.pushInstruction<Itype>(
                    addi, a0, x0, std::get<int>(node->ReturnVal));
            },
            // Else copy value into a0
            [&](VirtualRegister&) {
                CG.pushInstruction<MV>(
                    a0, std::get<VirtualRegister>(node->ReturnVal));
            },
            [&](std::string&) {
                CG.pushInstruction<MV>(
                    a0, std::get<std::string>(node->ReturnVal));
            },
        },
        node->ReturnVal);

    CG.pushInstruction<RET>();
}

void CodeGenerator::IRvisitor::operator()(
    const AssignmentNodePtr& node) {}

void CodeGenerator::IRvisitor::operator()(
    const AssignToVregPtr& node) {}

} // namespace CODEGEN