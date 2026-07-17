#include "code_emission.hpp"
#include "codegen.hpp"

std::string CODEGEN::generateASM(std::span<IR::IRnode> nodes,
                                 bool abi_regs) {
    auto generator = CODEGEN::CodeGenerator{nodes};

    auto emitter = CODEGEN::CodeEmitter{abi_regs};
    return emitter.emitAsm(generator.generateRISCVassembly());
}