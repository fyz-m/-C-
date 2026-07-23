#include "driver.hpp"

#include "IR/IRgen.hpp"
#include "IR/IRprinter.hpp"
#include "codegen/codegen.hpp"
#include "diagnostics/DiagnosticsEngine.hpp"
#include "lexer/lexer.hpp"
#include "parser/AstPrinter.hpp"
#include "parser/parser.hpp"

#include <cstring>
#include <fstream>
#include <print>
#include <sstream>

void run(const CompilerArgs& CompilerArgs) {

    std::ifstream file(CompilerArgs.InFilename,
                       std::ios::binary | std::ios::in);

    if (file.fail()) {
        std::println("Could not open file '{}'.",
                     CompilerArgs.InFilename);
        return;
    }

    std::stringstream text;
    text << file.rdbuf();
    auto source = text.str();

    using namespace Diagnostics;
    DiagnosticsEngine::init(source);

    auto tokens = Lexer(source).Tokenize();
    reportFailure(tokens.has_value());

    auto AST = Parser(tokens.value()).Parse();
    reportFailure(AST.has_value());
    if (CompilerArgs.Debug.PrintAst) {
        std::println(
            "------------------------------AST IR--------------"
            "---------------");
        AST::Printer::printAST(AST.value());
    }
    auto IRnodes = std::move(IR::Generator{AST.value()}.generateIR());

    if (CompilerArgs.Debug.PrintIR) {
        std::println(
            "------------------------------TAC IR--------------"
            "---------------");
        std::print("{}", IR::Printer::printIR(IRnodes));
    }
    if (CompilerArgs.Debug.PrintASM) {
        std::println(
            "------------------------------ASM IR--------------"
            "---------------");
        std::print("{}",
                   CODEGEN::generateASM(
                       IRnodes,
                       CompilerArgs.Debug.PrintABIregs,
                       !CompilerArgs.Debug.PrintPseudoInstructions));
    }
}

CompilerArgs ParseArgs(int argc, char* args[]) {

    CompilerArgs cargs{};

    if (argc < 2) {

        std::println("  Usage: ./main program.c  --flag");
        std::println("         --help for more.");
        return cargs;
    }

    static bool recieved_input_file{false};
    static bool printed_help{false};
    for (int i{1}; i < argc; ++i) {

        if (std::strcmp(args[i], "--AST") == 0) {
            cargs.Debug.PrintAst = true;
            continue;
        }

        if (std::strcmp(args[i], "--IR") == 0) {
            cargs.Debug.PrintIR = true;
            continue;
        }

        if (std::strcmp(args[i], "--ASM") == 0) {
            cargs.Debug.PrintASM = true;
            continue;
        }

        if (std::strcmp(args[i], "--no_abi") == 0) {
            cargs.Debug.PrintABIregs = false;
            continue;
        }

        if (std::strcmp(args[i], "--no_pseudo") == 0) {
            cargs.Debug.PrintPseudoInstructions = false;
            continue;
        }

        if (std::strcmp(args[i], "--o") == 0) {
            if (argc > i + 1) {
                cargs.OutFilename = args[++i];
                continue;
            }
            printFormattedArg("-o output_file_name",
                              "Specify name of output file");
            continue;
        }

        if (std::strcmp(args[i], "--help") == 0) {
            if (!printed_help)
                printHelp();
            printed_help = true;
            continue;
        }

        if (!recieved_input_file) {
            recieved_input_file = true;
            cargs.InvalidArgs = false;
            cargs.InFilename = args[i];
        }
    }

    if (!recieved_input_file && !printed_help)
        std::println("No file given for compilation.");
    return cargs;
}

void printHelp() {

    std::println("\nFlags:\n");

    printFormattedArg(
        "--o {output_file_name}",
        "Specify name of output file, default is \"output.asm\".\n");

    printFormattedArg("--AST",
                      "Print abtract-syntax-tree structure of"
                      " program.\n");

    printFormattedArg("--IR",
                      "Print three-address-code Intermediate"
                      " representation of program.\n");

    printFormattedArg("--ASM", "Print assembly output.\n");

    printFormattedArg("--no_abi",
                      "Print assembly output with numbered registers "
                      "instead of ABI names.\n");

    printFormattedArg(
        "--no_pseudo",
        "Print assembly output without pseudo-instructions\n");

    printFormattedArg("--help", "Print compiler flag details.\n");
}

void printFormattedArg(std::string_view arg, std::string_view desc) {
    std::string indent = "  ";
    std::println("{}{}\n{}{}{}", indent, arg, indent, indent, desc);
}

void reportFailure(bool hasValue) {
    using namespace Diagnostics;
    if (hasValue)
        return;

    DiagnosticsEngine::DisplayAllErrors();
    std::println("Compilation stopped with {} error(s).",
                 DiagnosticsEngine::errorCount());
}