#include "ASM/asm.hpp"

namespace CODEGEN {

// Visitor that converts ASM instruction into text
struct EmitAsm {
  private:
    bool m_PrintABIregs;

  public:
    EmitAsm(bool printABIregs)
        : m_PrintABIregs(printABIregs) {}

    std::string operator()(const ASM::AddPtr& inst) const;
    std::string operator()(const ASM::AddIPtr& inst) const;
    std::string operator()(const ASM::RetPtr& inst) const;
    std::string operator()(const ASM::MvPtr& inst) const;

  private:
    std::string str(ASM::REGISTER reg) const;
};

class CodeEmitter {

    EmitAsm m_EmitAsm{false};

  public:
    CodeEmitter(bool printABIregs = false)
        : m_EmitAsm{printABIregs} {}

    std::string emitAsm(std::span<ASM::Instruction> instrucions);
};

} // namespace CODEGEN