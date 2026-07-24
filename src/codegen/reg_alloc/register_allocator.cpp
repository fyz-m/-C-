#include "register_allocator.hpp"

namespace CODEGEN {

size_t RegisterAllocator::varToMem(RiscvInstructions& instructions) {
    StackAllocator allocator;
    allocator.varToStack(instructions);

    fixInstructions(instructions);
    // we unconditionally added 4 to the stackoffset
    return allocator.StackOffset;
}

void RegisterAllocator::fixInstructions(
    RiscvInstructions& instructions) {
    for (auto& instruction : instructions)
        fixInstruction(instruction);
}

void RegisterAllocator::fixInstruction(
    RISCV::Instruction& instruction) {
    using namespace RISCV;
    using namespace OPCODE;
    using enum I_TYPE;
    using enum S_TYPE;
    using enum REGISTER;

    if (auto* inst = std::get_if<PseudoInstrution>(&instruction)) {

        std::visit(
            Overloaded{

                [&](MvPtr& i) {
                    std::vector<RISCV::Instruction> replace;
                    auto* rd_stack = std::get_if<Stack>(&i->Rd);
                    auto* rs1_stack = std::get_if<Stack>(&i->Rs1);

                    if (rd_stack) {
                        replace.push_back(createInstruction<Itype>(
                            lw, t0, sp, rd_stack->Offset));
                    }
                    if (rs1_stack) {
                        auto reg = rd_stack ? t1 : t0;
                        replace.push_back(createInstruction<Itype>(
                            lw, reg, sp, rs1_stack->Offset));
                    }
                    replace.emplace_back(std::move(i));

                    if (rd_stack)
                        replace.push_back(createInstruction<Stype>(
                            sw, t0, rd_stack->Offset, sp));

                    instruction = createInstruction<InstructionList>(
                        std::move(replace));
                },

                [&](auto&) {}

            },
            *inst);

    } else {
        std::visit(Overloaded{[](RtypePtr&) {},
                              [](ItypePtr&) {},
                              [](StypePtr&) {},
                              [](BtypePtr&) {},
                              [](UtypePtr&) {},
                              [](JtypePtr&) {},
                              [](InstructionListPtr&) {},
                              [](PseudoInstrution&) {}},
                   instruction);
    }
}

void StackAllocator::varToStack(RiscvInstructions& instructions) {
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

void StackAllocator::operator()(const RISCV::StypePtr& inst) {
    assignStackToVar(inst->Rs1);
    assignStackToVar(inst->Rs2);
}

void StackAllocator::operator()(const RISCV::BtypePtr& inst) {
    assignStackToVar(inst->Rs1);
    assignStackToVar(inst->Rs2);
}
void StackAllocator::operator()(const RISCV::UtypePtr& inst) {
    assignStackToVar(inst->Rd);
}
void StackAllocator::operator()(const RISCV::JtypePtr& inst) {
    assignStackToVar(inst->Rd);
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

void StackAllocator::operator()(
    const RISCV::InstructionListPtr& inst) {
    varToStack(inst->Instructions);
}

void StackAllocator::assignStackToVar(RISCV::Operand& operand) {

    auto* var = std::get_if<IR::Variable>(&operand);
    if (!var)
        return;

    auto it = Map.find(*var);
    if (it == Map.end()) {
        Map.emplace(*var, StackOffset);
        // each variable is 4 bytes
        StackOffset += 4;
    }

    operand = Map[*var];
}

} // namespace CODEGEN