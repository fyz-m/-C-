#include <cstddef>
#include <string>
#include <variant>

enum class TokenType;

using LiteralValue = std::variant<std::monostate, int, float, std::string>;

struct Token {

    const TokenType type;
    const std::string lexeme{};
    const size_t line{};
    const size_t column{};
    const LiteralValue literal{};

    Token(TokenType type, std::string&& lexeme, size_t line, size_t column)
        : type{type}, lexeme{std::move(lexeme)}, line{line}, column(column) {}

    Token(TokenType type, std::string&& lexeme, size_t line, size_t column, LiteralValue&& literal)
        : type{type},
          lexeme{std::move(lexeme)},
          line{line},
          column(column),
          literal{std::move(literal)} {}

    Token(TokenType type, size_t line, size_t column)
        : type{type}, line{line}, column(column) {}
};

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literal
    IDENTIFIER,
    STR_LITERAL,
    INTEGER_LITERAL,
    FLOAT_LITERAL,

    // Keywords - control-flow
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    BREAK,

    // Keywords - types
    VOID,
    INT,
    FLOAT,

    END_OF_FILE
};