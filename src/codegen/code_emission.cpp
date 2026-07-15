#include "code_emission.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace CODEGEN {

std::string
CodeEmitter::emitAsm(std::span<ASM::Instruction> instrucions) {
    std::stringstream s;
    for (const auto& instruction : instrucions)
        s << std::visit(m_EmitAsm, instruction) << '\n';
    return s.str();
}

std::string EmitAsm::operator()(const ASM::AddPtr& inst) const {
    return std::format("add {}, {}, {}",
                       str(inst->Rd),
                       str(inst->Rs1),
                       str(inst->Rs2));
}

std::string EmitAsm::operator()(const ASM::AddIPtr& inst) const {
    return std::format("addi {}, {}, {}",
                       str(inst->Rd),
                       str(inst->Rs1),
                       std::to_string(inst->Imm));
}

std::string EmitAsm::operator()(const ASM::RetPtr& inst) const {
    return std::format("ret {}", str(inst->Reg));
}

std::string EmitAsm::operator()(const ASM::MvPtr& inst) const {
    return std::format("mv {}, {}", str(inst->Rd), str(inst->Rs1));
}

std::string EmitAsm::str(const ASM::REGISTER reg) const {

    if (!m_PrintABIregs)
        return std::format("x{}",
                           std::to_string(std::to_underlying(reg)));

    using namespace ASM;
    using enum REGISTER;

    switch (reg) {
    case x0:
        return "zero";
    case x1:
        return "ra";
    case x2:
        return "sp";
    case x3:
        return "gp";
    case x4:
        return "tp";
    case x5:
        return "t0";
    case x6:
        return "t1";
    case x7:
        return "t2";
    case x8:
        return "s0";
    case x9:
        return "s1";
    case x10:
        return "a0";
    case x11:
        return "a1";
    case x12:
        return "a2";
    case x13:
        return "a3";
    case x14:
        return "a4";
    case x15:
        return "a5";
    case x16:
        return "a6";
    case x17:
        return "a7";
    case x18:
        return "s2";
    case x19:
        return "s3";
    case x20:
        return "s4";
    case x21:
        return "s5";
    case x22:
        return "s6";
    case x23:
        return "s7";
    case x24:
        return "s8";
    case x25:
        return "s9";
    case x26:
        return "s10";
    case x27:
        return "s11";
    case x28:
        return "t3";
    case x29:
        return "t4";
    case x30:
        return "t5";
    case x31:
        return "t6";

    default:
        return "unknown";
    }
}

} // namespace CODEGEN