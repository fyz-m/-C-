#include "../codegen.hpp"

#include <unordered_map>

namespace CODEGEN {

// Allocator API so underlying allocator can be changed
class RegisterAllocator {

  public:
    static void varToMem(RiscvInstructions& instructions);
};

// Assigns all variables a place on the stack
struct StackAllocator {

    // so first stack offset is 0
    int StackOffset = -4;
    std::unordered_map<std::string, RISCV::Stack> Map;

    void allocateRegs(RiscvInstructions& instructions);

    void operator()(const RISCV::ItypePtr& inst);
    void operator()(const RISCV::RtypePtr& inst);
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