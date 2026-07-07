#pragma once
#include <cstddef>
#include <string>
#include <variant>

enum class TokenType;

using LiteralValue = std::variant<std::monostate, int, float, std::string>;

struct Location {
    size_t line{};
    size_t col_start{};
    size_t col_end{};
};

struct Token {

    TokenType type;
    std::string lexeme{};
    Location loc;
    LiteralValue literal{};

    Token(TokenType type, std::string&& lexeme, Location location)
        : type{type}, lexeme{std::move(lexeme)}, loc{location} {}

    Token(TokenType type, std::string&& lexeme, Location location, LiteralValue&& literal)
        : type{type}, lexeme{std::move(lexeme)}, loc{location}, literal{std::move(literal)} {}

    Token(TokenType type, Location location)
        : type{type}, loc{location} {}
};

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    TILDE,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    MINUS,
    MINUS_MINUS,
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
    DEF,

    // Keywords - types
    VOID,
    INT,
    FLOAT,

    END_OF_FILE
};
