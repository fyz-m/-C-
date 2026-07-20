#include "expand_pseudo.hpp"

namespace CODEGEN {

using namespace RISCV;
using namespace OPCODE;
using enum R_TYPE;
using enum I_TYPE;
using enum REGISTER;

void convertPseudoInstructions(RiscvInstructions& instructions) {
    for (auto& instr : instructions)
        convertPseudoInstruction(instr);
}

void convertPseudoInstruction(Instruction& instruction) {

    if (auto* p = std::get_if<PseudoInstrution>(&instruction)) {

        std::visit(
            Overloaded{

                [&](const MvPtr& i) {
                    // mv rd, rs1 -> addi rd, rs1, 0
                    instruction = createInstruction<Itype>(
                        addi, std::move(i->Rd), std::move(i->Rs1), 0);
                },

                // ret -> jalr x0, ra, 0
                [&](const RetPtr& i) {},

                // I-type instructions in RISC-V can only encode a
                // 12-bit immediate
                [&](const LIPtr& i) {
                    // if imm > 12 bits
                    // Li x1, 2046 -> lui  + addi
                    if (std::bit_width(
                            static_cast<uint32_t>(i->Imm)) > 12) {
                        return;
                    }
                    // else
                    // addi rd, 0, imm
                    instruction = createInstruction<Itype>(
                        addi, std::move(i->Rd), zero, i->Imm);
                },

                [&](const NegPtr& i) {
                    instruction =
                        createInstruction<Rtype>(sub,
                                                 std::move(i->Rd),
                                                 zero,
                                                 std::move(i->Rs1));
                },

                [&](const NotPtr& i) {
                    // XOR'ing rs1 with all 1's will invert all
                    // the bits in rs1
                    instruction =
                        createInstruction<Itype>(xori,
                                                 std::move(i->Rd),
                                                 std::move(i->Rs1),
                                                 -1);
                },

            },
            *p);
    }
}
} // namespace CODEGEN