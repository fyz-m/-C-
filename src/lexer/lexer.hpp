#include "Token.hpp"

#include <cstddef>
#include <expected>
#include <string_view>
#include <unordered_map>
#include <vector>

class Lexer {

  private:
    // These act as pointers to keep track of the location
    // the scanner is at in the source code
    size_t m_start{};
    size_t m_current{};

    // Used for error reporting
    size_t m_line{1};
    size_t m_column_start{1};
    size_t m_column_end{1};

    std::string_view m_source;
    std::vector<Token> m_tokens;

    bool m_hadError{false};

  public:
    Lexer(std::string_view source);

    std::expected<std::vector<Token>, bool> Tokenize();

  private:
    void scanToken();

    void addToken(TokenType type);

    void addToken(TokenType type, LiteralValue& literal);

    // Return current subscript of source
    std::string getLexeme() const;

    Location getLocation() const;

    void number();

    void identifier();

    void string();

    bool isatEnd() const;

    size_t incrementLine();

    // Add line to diagnostic engine's line tracker
    void addLineToDE() const;

    // Return char at location m_current and increment m_current (consume char)
    char advance();

    // Conditional advance/consume
    bool match(char expected);

    // Return char at location m_current, does not increment m_current
    char peek() const;

    char peekNext() const;

    std::unordered_map<std::string, TokenType> m_KEYWORDS{{{"else", TokenType::ELSE},
                                                           {"for", TokenType::FOR},
                                                           {"if", TokenType::IF},
                                                           {"return", TokenType::RETURN},
                                                           {"while", TokenType::WHILE},
                                                           {"break", TokenType::BREAK},
                                                           {"void", TokenType::VOID},
                                                           {"int", TokenType::INT},
                                                           {"float", TokenType::FLOAT}}};
};
