#pragma once
#include "../lexer/Token.hpp"

#include <string_view>
#include <vector>

namespace Diagnostics {

enum class ErrorType { LexerError, SyntaxError };
enum class Severity { Fatal, Warning };

// Stores where each line starts/ends as indices into the source string buffer
struct LineInformation {
    size_t start{};
    size_t end{};
};

struct Diagnostic {
    Location loc{};
    std::string_view errorMessage{};
    ErrorType type{};
};

class DiagnosticsEngine {
  private:
    // String buffer holding the source code file contents
    inline static std::string_view m_source;
    // Keep track of each line so entire line can be printed in error
    inline static std::vector<LineInformation> m_lineInfo;
    inline static std::vector<Diagnostic> m_Errors;
    inline static size_t m_errorCount;

  public:
    static void init(std::string_view m_source);
    static void DisplayAllErrors();
    static void addLine(size_t lineStart, size_t lineEnd);
    static void report(std::string_view errorMessage, size_t line, size_t column);
    static void report(Token& token, std::string_view errorMessage);
    static size_t errorCount();

    DiagnosticsEngine() = delete;

  private:
    static void printLn(size_t line);
    static void renderDiagnostic(Diagnostic& diagnostic);
};

} // namespace Diagnostics
