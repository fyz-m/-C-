#include "IR/IRgen.hpp"
#include "IR/IRprinter.hpp"
#include "codegen/backend_driver.hpp"
#include "diagnostics/DiagnosticsEngine.hpp"
#include "lexer/lexer.hpp"
#include "parser/AstPrinter.hpp"
#include "parser/parser.hpp"

#include <cstring>
#include <fstream>
#include <print>
#include <sstream>

void printHelp();

void reportFailure(bool hasValue);

void printFormattedArg(std::string_view arg, std::string_view desc);

struct CompilerArgs {
    std::string InFilename;
    std::string OutFilename = "output.asm";
    bool PrintAst{false};
    bool PrintIR{false};
    bool PrintASM{false};
    bool InvalidArgs{true};
};

void run(const CompilerArgs& CompilerArgs);

CompilerArgs ParseArgs(int argc, char* args[]);

int main(int argc, char* argv[]) {

    auto compiler_args = ParseArgs(argc, argv);
    if (compiler_args.InvalidArgs)
        return 1;

    try {
        run(compiler_args);
    } catch (const std::runtime_error& e) {
        std::println("{}", e.what());
        return 1;
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
    for (int i{1}; i < argc; ++i) {

        if (std::strcmp(args[i], "--AST") == 0) {
            cargs.PrintAst = true;
            continue;
        }

        if (std::strcmp(args[i], "--IR") == 0) {
            cargs.PrintIR = true;
            continue;
        }

        if (std::strcmp(args[i], "--ASM") == 0) {
            cargs.PrintASM = true;
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
            static bool alr_printed{false};
            if (!alr_printed)
                printHelp();
            alr_printed = true;
            continue;
        }

        if (!recieved_input_file) {
            recieved_input_file = true;
            cargs.InvalidArgs = false;
            cargs.InFilename = args[i];
        }
    }

    if (!recieved_input_file)
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
                      "representation of program.\n");

    printFormattedArg("--ASM", "Print assembly output.\n");

    printFormattedArg("--help", "Print compiler flag details.\n");
}

void printFormattedArg(std::string_view arg, std::string_view desc) {
    std::string indent = "  ";
    std::println("{}{}\n{}{}{}", indent, arg, indent, indent, desc);
}

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
    if (CompilerArgs.PrintAst) {
        std::println(
            "------------------------------AST IR--------------"
            "---------------");
        AST::Printer::printAST(AST.value());
    }
    auto IRnodes = std::move(IR::Generator{AST.value()}.generateIR());

    if (CompilerArgs.PrintIR) {
        std::println(
            "------------------------------TAC IR--------------"
            "---------------");
        std::print("{}", IR::Printer::printIR(IRnodes));
    }
    if (CompilerArgs.PrintASM) {
        std::println(
            "------------------------------ASM IR--------------"
            "---------------");
        std::print("{}", generateASM(IRnodes, true));
    }
}

void reportFailure(bool hasValue) {
    using namespace Diagnostics;
    if (hasValue)
        return;

    DiagnosticsEngine::DisplayAllErrors();
    throw std::runtime_error(
        std::format("Compilation stopped with {} error(s).",
                    DiagnosticsEngine::errorCount()));
}