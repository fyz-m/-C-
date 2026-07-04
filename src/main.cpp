#include "lexer/lexer.hpp"
#include "parser/AstPrinter.hpp"
#include "parser/parser.hpp"

#include <fstream>
#include <print>
#include <sstream>

void run(const std::string& filepath);

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::println("Usage: ./main program.c");
        return 1;
    }
    run(argv[1]);
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
    Lexer lexer(source);
    auto tokens = lexer.Tokenize();

    if (!tokens.has_value()) {
        std::println("Lexer error");
        return;
    }

    Parser parser = Parser(tokens.value());
    auto AST = parser.Parse();
    AST::Printer::printAST(AST);
}
