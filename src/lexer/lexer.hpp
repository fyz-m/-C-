#include "Token.hpp"

#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <vector>

class Lexer {

  private:
    size_t m_start{};
    size_t m_current{};
    size_t m_line{1};
    size_t m_column{1};

    std::string_view m_source;
    std::vector<Token> m_tokens;

    bool m_hadError{false};

  public:
    Lexer(std::string_view source);

    std::vector<Token> Tokenize();

  private:
    void scanToken();

    void addToken(TokenType type);

    void addToken(TokenType type, LiteralValue& literal);

    // Return current subscript of source
    std::string getLexeme();

    void number();

    void identifier();

    void string();

    bool isatEnd() const;

    size_t incrementLine();

    // Return char at location m_current and increment m_current (consume char)
    char advance();

    // Conditional advance/consume
    bool match(char expected);

    // Return char at location m_current, does not increment m_current
    char peek() const;

    char peekNext() const;

    bool isAlpha(char c);

    bool isAlphanumeric(char c);
};

const std::unordered_map<std::string, TokenType> KEYWORDS{{{"else", TokenType::ELSE},
                                                           {"for", TokenType::FOR},
                                                           {"if", TokenType::IF},
                                                           {"return", TokenType::RETURN},
                                                           {"while", TokenType::WHILE},
                                                           {"break", TokenType::BREAK},
                                                           {"void", TokenType::VOID},
                                                           {"int", TokenType::INT},
                                                           {"float", TokenType::FLOAT}}};