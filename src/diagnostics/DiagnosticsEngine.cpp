#include "DiagnosticsEngine.hpp"

namespace Diagostics {

void DiagnosticsEngine::init(std::string_view source) {
    m_source = source;
    m_lineInfo.reserve(100);

    // Filler at [0] so index corresponds with line number
    // E.g line 2's information is at m_lineInfo[2]
    m_lineInfo.emplace_back(0, 0);
}

void DiagnosticsEngine::report(Token& token, std::string_view errorMessage, ErrorType type) {}

void DiagnosticsEngine::addLine(size_t lineStart, size_t lineEnd) {
    m_lineInfo.emplace_back(lineStart, lineEnd);
}
Diagnostic DiagnosticsEngine::createDiagnostic(Token& token, std::string_view errorMessage,
                                               ErrorType type) {}

void DiagnosticsEngine::renderDiagnostic(Diagnostic& diagnostic) {}

} // namespace Diagostics