#include "../codegen.hpp"

#include <unordered_map>

namespace CODEGEN {

// Allocator API so underlying allocator can be changed
class RegisterAllocator {

  public:
    // Assigns all variables a place on the stack, returns stack size

    static size_t varToMem(RiscvInstructions& instructions);

    // RISC-V instructions can't act on memory addresses directly,
    // so we need to load the stack operand into a register first
    static void fixInstructions(RiscvInstructions& instructions);

  private:
    static void fixInstruction(RISCV::Instruction& instruction);
};

// Assigns all variables a place on the stack
struct StackAllocator {

    size_t StackOffset{};
    std::unordered_map<std::string, RISCV::Stack> Map;

    void varToStack(RiscvInstructions& instructions);

    void operator()(const RISCV::RtypePtr& inst);
    void operator()(const RISCV::ItypePtr& inst);
    void operator()(const RISCV::StypePtr& inst);
    void operator()(const RISCV::BtypePtr& inst);
    void operator()(const RISCV::UtypePtr& inst);
    void operator()(const RISCV::JtypePtr& inst);

    void operator()(const RISCV::InstructionListPtr& inst);

    void operator()(const RISCV::PseudoInstrution& inst);
    void operator()(const RISCV::RetPtr& inst);
    void operator()(const RISCV::MvPtr& inst);
    void operator()(const RISCV::LIPtr& inst);
    void operator()(const RISCV::NotPtr& inst);
    void operator()(const RISCV::NegPtr& inst);

  private:
    void assignStackToVar(RISCV::Operand& operand);
};

} // namespace CODEGEN