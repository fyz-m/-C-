#include "code_emission.hpp"
#include "codegen.hpp"
#include "reg_alloc/register_allocator.hpp"

std::string CODEGEN::generateASM(std::span<IR::IRnode> nodes,
                                 bool abi_regs) {
    auto generator = CODEGEN::CodeGenerator{nodes};
    generator.generateRISCVassembly();

    auto& instructions = generator.getInstructions();
    // CODEGEN::RegisterAllocator().varToMem(instructions);

    auto emitter = CODEGEN::CodeEmitter{abi_regs};
    return emitter.emitAsm(instructions);
}