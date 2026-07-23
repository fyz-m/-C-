#pragma once
#include <string>
#include <string_view>

void reportFailure(bool hasValue);

void printFormattedArg(std::string_view arg, std::string_view desc);

void printHelp();

struct CompilerArgs {

    std::string InFilename;
    std::string OutFilename = "output.asm";

    struct Debug {
        bool PrintAst{false};
        bool PrintIR{false};
        bool PrintASM{false};
        bool PrintABIregs{true};
        bool PrintPseudoInstructions{true};
    };
    Debug Debug{};

    bool InvalidArgs{true};
};

CompilerArgs ParseArgs(int argc, char* args[]);

void run(const CompilerArgs& CompilerArgs);