#include "DiagnosticsEngine.hpp"

#include <print>

namespace Diagnostics {

void DiagnosticsEngine::init(std::string_view source) {

    static bool initialized{false};
    if (initialized)
        return;

    m_source = source;
    m_lineInfo.reserve(100);

    // Filler at [0] so index corresponds with line number
    // E.g line 2's information is at m_lineInfo[2]
    m_lineInfo.emplace_back(0, 0);
    initialized = true;
}

void DiagnosticsEngine::printLn(size_t line) {
    auto& info = m_lineInfo.at(line);
    std::print("{}", m_source.substr(info.start, info.end - info.start));
}

void DiagnosticsEngine::report(Token& token, std::string_view errorMessage, ErrorType type) {}

void DiagnosticsEngine::report(std::string_view errorMessage, size_t line, size_t column) {
    m_Errors.emplace_back(Location(line, column, column), errorMessage, ErrorType::LexerError);
}

void DiagnosticsEngine::addLine(size_t lineStart, size_t lineEnd) {
    m_lineInfo.emplace_back(lineStart, lineEnd);
}

Diagnostic DiagnosticsEngine::createDiagnostic(Token& token, std::string_view errorMessage,
                                               ErrorType type) {
    return Diagnostic();
}

void DiagnosticsEngine::DisplayAllErrors() {
    for (auto& error : m_Errors) {
        renderDiagnostic(error);
    }
}

void DiagnosticsEngine::renderDiagnostic(Diagnostic& diagnostic) {

    auto line_col_ = std::format("{}:{}c", diagnostic.loc.line, diagnostic.loc.col_start);

    std::println("{} | Error: {}", line_col_, diagnostic.errorMessage);
    std::println("{} |", std::string(line_col_.length(), ' '));
    std::print("{} | ", std::string(line_col_.length(), ' '));

    DiagnosticsEngine::printLn(diagnostic.loc.line);

    size_t length = diagnostic.loc.col_end - diagnostic.loc.col_start;
    if (length == 0)
        length = 1;

    if (diagnostic.loc.line == m_lineInfo.size() - 1)
        std::println();

    std::print("{} | ", std::string(line_col_.length(), ' '));
    std::println("{}{} Here", std::string(diagnostic.loc.col_start - 1, ' '),
                 std::string(length, '^'));

    std::println();
}

} // namespace Diagnostics