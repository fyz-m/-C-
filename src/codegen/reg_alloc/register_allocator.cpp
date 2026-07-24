#include "register_allocator.hpp"

namespace CODEGEN {

void RegisterAllocator::varToMem(RiscvInstructions& instructions) {
    StackAllocator allocator;
    allocator.allocateRegs(instructions);
}

void StackAllocator::allocateRegs(RiscvInstructions& instructions) {
    for (const auto& instruction : instructions)
        std::visit(*this, instruction);
}

void StackAllocator::operator()(const RISCV::ItypePtr& inst) {
    assignStackToVar(inst->Rd);
    assignStackToVar(inst->Rs1);
}

void StackAllocator::operator()(const RISCV::RtypePtr& inst) {
    assignStackToVar(inst->Rd);
    assignStackToVar(inst->Rs1);
    assignStackToVar(inst->Rs2);
}

void StackAllocator::operator()(
    const RISCV::InstructionListPtr& inst) {
    allocateRegs(inst->Instructions);
}

void StackAllocator::operator()(const RISCV::PseudoInstrution& inst) {
    std::visit(*this, inst);
}

void StackAllocator::operator()(const RISCV::RetPtr&) {}

void StackAllocator::operator()(const RISCV::MvPtr& inst) {
    assignStackToVar(inst->Rd);
    assignStackToVar(inst->Rs1);
}

void StackAllocator::operator()(const RISCV::LIPtr& inst) {
    assignStackToVar(inst->Rd);
}

void StackAllocator::operator()(const RISCV::NotPtr& inst) {
    assignStackToVar(inst->Rd);
    assignStackToVar(inst->Rs1);
}

void StackAllocator::operator()(const RISCV::NegPtr& inst) {
    assignStackToVar(inst->Rd);
    assignStackToVar(inst->Rs1);
}

void StackAllocator::assignStackToVar(RISCV::Operand& operand) {

    auto* var = std::get_if<IR::Variable>(&operand);
    if (!var)
        return;

    auto it = Map.find(*var);
    if (it == Map.end())
        Map.emplace(*var, StackOffset += 4);

    operand = Map[*var];
}

} // namespace CODEGEN