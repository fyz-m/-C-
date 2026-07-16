#include "codegen.hpp"

#include "ASM/CreateASMnode.hpp"

using namespace RISCV;
using namespace IR;
using RISCV::OPCODE;
using enum RISCV::OPCODE::I_TYPE;
using enum REGISTER;

namespace CODEGEN {

std::vector<RISCV::Instruction>&
CodeGenerator::generateRISCVassembly() {
    for (const auto& IRnode : m_IRnodes)
        std::visit(IRvisitor{*this}, IRnode);
    return m_Instructions;
}

void CodeGenerator::IRvisitor::operator()(const BinaryNodePtr& node) {
}

void CodeGenerator::IRvisitor::operator()(const UnaryNodePtr& node) {

    if (node->Op == OPERATION::CMPLMNT) {

        // t0 = ~5 ->
        // li t0, 5
        // not t0, t0
        if (auto* imm = std::get_if<int>(&node->Src1)) {
            CG.pushInstruction<LI>(node->Result, *imm);
            CG.pushInstruction<NOT>(node->Result, node->Result);
            return;
        }
        // t0 = ~t1 ->
        // not t0, t1
        CG.pushInstruction<NOT>(
            node->Result, CodeGenerator::castVariant(node->Src1));
    }
}

void CodeGenerator::IRvisitor::operator()(const ReturnNodePtr& node) {

    // Place return value in a0 register
    // a0 is the return value register in RISC-V convention

    // ret 5 ->
    // addi a0, x0, 5
    // ret
    if (auto* imm = std::get_if<int>(&node->ReturnVal)) {
        CG.pushInstruction<Itype>(addi, a0, x0, *imm);
    }
    // ret t0 ->
    // mv a0, t0
    // ret
    else {
        CG.pushInstruction<MV>(
            a0, CodeGenerator::castVariant(node->ReturnVal));
    }

    CG.pushInstruction<RET>();
}

void CodeGenerator::IRvisitor::operator()(
    const AssignmentNodePtr& node) {}

void CodeGenerator::IRvisitor::operator()(
    const AssignToVregPtr& node) {}

} // namespace CODEGEN