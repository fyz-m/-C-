#include "lexer/lexer.hpp"

#include <fstream>
#include <print>
#include <sstream>

void run(const std::string& filepath);

int main(int argc, char* argv[]) {

    auto* filepath = argv[1];
    run(filepath);
}

void run(const std::string& filepath) {
    std::ifstream file(filepath);
    if (file.fail()) {
        std::println("Could not open file '{}'.", filepath);
        return;
    }

    std::stringstream text;
    text << file.rdbuf();

    Lexer lexer;
    lexer.Tokenize(text.str());
}
