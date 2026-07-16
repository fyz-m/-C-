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
    std::string operator()(const RISCV::PseudoInstrution& inst) const;
    std::string operator()(const RISCV::RetPtr& inst) const;
    std::string operator()(const RISCV::MvPtr& inst) const;
    std::string operator()(const RISCV::LIptr& inst) const;
    std::string operator()(const RISCV::Notptr& inst) const;
    std::string operator()(const RISCV::Negptr& inst) const;

  private:
    [[nodiscard]] std::string
    operandToStr(RISCV::Operand& operand) const;

    [[nodiscard]] constexpr std::string
    regToStr(RISCV::REGISTER reg) const;

    [[nodiscard]] constexpr std::string
    opToStr(RISCV::OPCODE::I_TYPE opcode) const;
    [[nodiscard]] constexpr std::string
    opToStr(RISCV::OPCODE::R_TYPE opcode) const;
};

class CodeEmitter {

    EmitAsm m_EmitAsm{false};

  public:
    CodeEmitter(bool printABIregs = false)
        : m_EmitAsm{printABIregs} {}

    std::string emitAsm(std::span<RISCV::Instruction> instrucions);
};

} // namespace CODEGEN