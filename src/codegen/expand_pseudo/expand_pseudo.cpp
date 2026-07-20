#include "expand_pseudo.hpp"

namespace CODEGEN {

void convertPseudoInstructions(RiscvInstructions& instructions) {

    using namespace RISCV;
    using namespace OPCODE;
    using enum R_TYPE;
    using enum I_TYPE;
    using enum REGISTER;

    for (auto& instr : instructions) {

        if (const auto* pinstr =
                std::get_if<PseudoInstrution>(&instr)) {

            std::visit(
                Overloaded{

                    [&](const MvPtr& i) {
                        // mv rd, rs1 -> addi rd, rs1, 0
                        instr = createInstruction<Itype>(
                            addi,
                            std::move(i->Rd),
                            std::move(i->Rs1),
                            0);
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
                        instr = createInstruction<Itype>(
                            addi, std::move(i->Rd), zero, i->Imm);
                    },

                    [&](const NegPtr& i) {

                    },

                    [&](const NotPtr& i) {
                        // XOR'ing rs1 with all 1's will invert all
                        // the bits in rs1
                        instr = createInstruction<Itype>(
                            xori,
                            std::move(i->Rd),
                            std::move(i->Rs1),
                            -1);
                    },

                },
                *pinstr);
        }
    }
}

} // namespace CODEGEN