#pragma once
#include "../lexer/Token.hpp"

#include <string_view>
#include <vector>

namespace Diagostics {

struct LineInformation;
struct Diagnostic;

enum class ErrorType { LexerError, SyntaxError };
enum class Severity { Fatal, Warning };

class DiagnosticsEngine {
  private:
    // String buffer holding the source code file contents
    static std::string_view m_source;
    // Keep track of each line so entire line can be printed in error
    static std::vector<LineInformation> m_lineInfo;
    static size_t m_errorCount;

  public:
    static void init(std::string_view m_source);
    static void report(Token& token, std::string_view errorMessage, ErrorType type);
    static void addLine(size_t lineStart, size_t lineEnd);

    DiagnosticsEngine() = delete;

  private:
    Diagnostic createDiagnostic(Token& token, std::string_view errorMessage, ErrorType type);
    void renderDiagnostic(Diagnostic& diagnostic);
};

// Stores where each line starts/ends as indices into the source string buffer
struct LineInformation {
    size_t start;
    size_t end;
};

struct Diagnostic {
    Token* token;
    std::string_view errorMessage;
    ErrorType type;
};

} // namespace Diagostics
