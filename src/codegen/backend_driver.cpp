#include "code_emission.hpp"
#include "codegen.hpp"
#include "expand_pseudo/expand_pseudo.hpp"
#include "reg_alloc/register_allocator.hpp"

std::string CODEGEN::generateASM(std::span<IR::IRnode> nodes,
                                 bool abi_regs,
                                 bool convertPseudo) {
    auto generator = CODEGEN::CodeGenerator{nodes};
    generator.generateRISCVassembly();

    auto& instructions = generator.getInstructions();
    // CODEGEN::RegisterAllocator().varToMem(instructions);

    if (convertPseudo)
        convertPseudoInstructions(instructions);

    auto emitter = CODEGEN::CodeEmitter{abi_regs};
    return emitter.emitAsm(instructions);
}