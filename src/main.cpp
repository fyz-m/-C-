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

void run(const std::string& filepath);

void printHelp();

void printFormattedArg(std::string_view arg, std::string_view desc);

struct CompilerArgs {
    std::string InFilename;
    std::string OutFilename = "output.asm";
    bool InvalidArgs{true};
    bool PrintAst{false};
    bool PrintIR{false};
    bool PrintASM{false};
};

CompilerArgs ParseArgs(int argc, char* args[]);

int main(int argc, char* argv[]) {

    auto compiler_args = ParseArgs(argc, argv);
    if (compiler_args.InvalidArgs)
        return 1;
    run(compiler_args.InFilename);
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
}

void printFormattedArg(std::string_view arg, std::string_view desc) {
    std::string indent = "  ";
    std::println("{}{}\n{}{}{}", indent, arg, indent, indent, desc);
}

void run(const std::string& filepath) {

    std::ifstream file(filepath, std::ios::binary | std::ios::in);
    if (file.fail()) {
        std::println("Could not open file '{}'.", filepath);
        return;
    }

    std::stringstream text;
    text << file.rdbuf();
    auto source = text.str();

    Diagnostics::DiagnosticsEngine::init(source);

    Lexer lexer(source);
    auto tokens = lexer.Tokenize();

    if (!tokens.has_value()) {
        Diagnostics::DiagnosticsEngine::DisplayAllErrors();
        return;
    }

    Parser parser = Parser(tokens.value());
    auto AST = parser.Parse();

    if (auto ec = Diagnostics::DiagnosticsEngine::errorCount()) {
        std::println("\nStopping with {} error(s):\n", ec);
        Diagnostics::DiagnosticsEngine::DisplayAllErrors();
        return;
    }
    AST::Printer::printAST(AST.value());
    IR::Generator generator = IR::Generator{AST.value()};
    auto& IRnodes = generator.generateIR();

    std::println("                    IR GENERATED");
    std::println("{}",
                 "---------------------------------------------------"
                 "----------------------------");
    std::print("{}", IR::Printer::printIR(IRnodes));

    std::println("                    ASM GENERATED");
    std::println("{}",
                 "---------------------------------------------------"
                 "----------------------------");
    std::print("{}", generateASM(IRnodes, true));
}
