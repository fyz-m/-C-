#include "../codegen.hpp"

#include <unordered_map>

namespace CODEGEN {

// Allocator API so underlying allocator can be changed
class RegisterAllocator {

  public:
    RegisterAllocator();

    // Place all variables onto the stack
    static void varToMem(RiscvInstructions& instructions);
};

// Naive allocator, spills everything to the stack
struct NaiveAllocator {
    // so first stack offset is 0
    int StackOffset = -4;
    std::unordered_map<std::string, RISCV::Stack> Map;

    void allocateRegs(RiscvInstructions& instructions);

    void operator()(const RISCV::ItypePtr& inst);
    void operator()(const RISCV::RtypePtr& inst);

    void operator()(const RISCV::PseudoInstrution&);
    // void operator()(const RISCV::RetPtr& inst) const;
    // void operator()(const RISCV::MvPtr& inst) const;
    // void operator()(const RISCV::LIPtr& inst) const;
    // void operator()(const RISCV::NotPtr& inst) const;
    // void operator()(const RISCV::NegPtr& inst) const;

    void assignStackToVar(RISCV::Operand& operand);
};

} // namespace CODEGEN