#include "ASM/RISC-V.hpp"
#include "codegen/ASM/RISC-V.hpp"

#include <string>

namespace CODEGEN {

// Visitor that converts ASM instruction into text
struct EmitAsm {
  private:
    bool m_PrintABIregs;

  public:
    EmitAsm(bool printABIregs)
        : m_PrintABIregs(printABIregs) {}

    std::string operator()(const RISCV::RtypePtr& inst) const;
    std::string operator()(const RISCV::ItypePtr& inst) const;
    std::string operator()(const RISCV::StypePtr& inst) const;
    std::string operator()(const RISCV::BtypePtr& inst) const;
    std::string operator()(const RISCV::UtypePtr& inst) const;
    std::string operator()(const RISCV::JtypePtr& inst) const;

    std::string
    operator()(const RISCV::InstructionListPtr& inst) const;

    // Used for debugging, all pseudo instructions are usually
    // converted to normal RISC-V instructions before code emission
    std::string operator()(const RISCV::PseudoInstrution& inst) const;
    std::string operator()(const RISCV::RetPtr& inst) const;
    std::string operator()(const RISCV::MvPtr& inst) const;
    std::string operator()(const RISCV::LIPtr& inst) const;
    std::string operator()(const RISCV::NotPtr& inst) const;
    std::string operator()(const RISCV::NegPtr& inst) const;

  private:
    bool isLoadInstruction(RISCV::OPCODE::I_TYPE opcode) const;

    [[nodiscard]] std::string
    operandToStr(RISCV::Operand& operand) const;

    [[nodiscard]] std::string regToStr(RISCV::REGISTER reg) const;

    [[nodiscard]] std::string
    opToStr(RISCV::OPCODE::I_TYPE opcode) const;

    [[nodiscard]] std::string
    opToStr(RISCV::OPCODE::R_TYPE opcode) const;

    [[nodiscard]] std::string
    opToStr(RISCV::OPCODE::S_TYPE opcode) const;

    [[nodiscard]] std::string
    opToStr(RISCV::OPCODE::B_TYPE opcode) const;

    [[nodiscard]] std::string
    opToStr(RISCV::OPCODE::U_TYPE opcode) const;

    [[nodiscard]] std::string
    opToStr(RISCV::OPCODE::J_TYPE opcode) const;
};

class CodeEmitter {

    bool m_PrintABIregs;

  public:
    CodeEmitter(bool printABIregs = false)
        : m_PrintABIregs{printABIregs} {}

    std::string
    emitAsm(std::span<RISCV::Instruction> instrucions) const;
};

} // namespace CODEGEN