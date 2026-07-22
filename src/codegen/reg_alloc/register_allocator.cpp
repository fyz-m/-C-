#include "register_allocator.hpp"

namespace CODEGEN {

void RegisterAllocator::varToMem(RiscvInstructions& instructions) {
    NaiveAllocator allocator;
    allocator.allocateRegs(instructions);
}

void NaiveAllocator::allocateRegs(RiscvInstructions& instructions) {
    for (const auto& instruction : instructions)
        std::visit(*this, instruction);
}

void NaiveAllocator::operator()(const RISCV::ItypePtr& inst) {
    assignStackToVar(inst->Rd);
    assignStackToVar(inst->Rs1);
}

void NaiveAllocator::operator()(const RISCV::RtypePtr& inst) {
    assignStackToVar(inst->Rd);
    assignStackToVar(inst->Rs1);
    assignStackToVar(inst->Rs2);
}

void NaiveAllocator::operator()(const RISCV::PseudoInstrution&) {
    throw std::runtime_error(
        "Attempt to allocate register to pseudo instruction.");
}

void NaiveAllocator::assignStackToVar(RISCV::Operand& operand) {

    auto* var = std::get_if<IR::Variable>(&operand);
    if (!var)
        return;

    auto it = Map.find(*var);
    if (it == Map.end())
        Map.emplace(*var, StackOffset += 4);

    operand = Map[*var];
}

} // namespace CODEGEN