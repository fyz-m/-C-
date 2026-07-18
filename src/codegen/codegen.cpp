#include "codegen.hpp"

using namespace RISCV;
using namespace IR;

namespace CODEGEN {

void CodeGenerator::generateRISCVassembly() {
    // Each IR node will approximately be equivalent to 2 RISC-V
    // instructions, so reserve to minimize resizing
    // of vector.
    IRvisitor visitor = IRvisitor{m_IRnodes.size() * 2};

    for (const auto& node : m_IRnodes)
        std::visit(visitor, node);

    m_Instructions = std::move(visitor.Instructions);
}

RiscvInstructions& CodeGenerator::getInstructions() {
    return m_Instructions;
}

using namespace OPCODE;
using enum I_TYPE;
using enum R_TYPE;
using enum REGISTER;

void IRvisitor::operator()(const BinaryNodePtr& node) {}

void IRvisitor::operator()(const UnaryNodePtr& node) {

    RISCV::Operand src1;

    // t0 = ~5 ->
    // li t0, 5
    // not t0, t0
    if (auto* imm = std::get_if<int>(&node->Src1)) {
        // TODO:
        // add a loadImmediate function to avoid pseudo instructions
        pushInstruction<LI>(node->Result, *imm);
        src1 = node->Result;

    } // t0 = ~t1 ->
      // not t0, t1
    else {
        castVariant(node->Src1);
    }

    switch (node->Op) {

    case OPERATION::CMPLMNT:
        pushInstruction<NOT>(node->Result, src1);
        return;
    case OPERATION::NEG:
        // If imm < 12 bits, we can just use subi rd, x0, imm
        pushInstruction<NEG>(node->Result, src1);
        return;

    default:
        // unreachable
        return;
    }
}

// TODO:
// handle no return value (return;)
void IRvisitor::operator()(const ReturnNodePtr& node) {

    // Place return value in a0 register
    // a0 is the return value register in RISC-V convention

    // ret 5 ->
    // addi a0, x0, 5
    // ret
    if (auto* imm = std::get_if<int>(&node->ReturnVal)) {
        pushInstruction<Itype>(addi, a0, x0, *imm);
    }
    // ret t0 ->
    // mv a0, t0
    // ret
    else {
        pushInstruction<MV>(a0, castVariant(node->ReturnVal));
    }

    pushInstruction<RET>();
}

void IRvisitor::operator()(const AssignmentNodePtr& node) {}

void IRvisitor::operator()(const AssignToVregPtr& node) {}

} // namespace CODEGEN