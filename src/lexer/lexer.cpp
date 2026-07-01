#include "lexer.hpp"

Lexer::Lexer(std::string_view source)
    : m_source{source} {
    m_tokens.reserve(1000 * sizeof(Token));
}

std::vector<Token> Lexer::Tokenize() {

    while (!isatEnd()) {
        m_start = m_current;
        scanToken();
    }

    m_tokens.emplace_back(TokenType::END_OF_FILE, m_line, m_column);
    return m_tokens;
}

void Lexer::scanToken() {

    char c = advance();

    switch (c) {
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;
    case '{':
        addToken(TokenType::LEFT_BRACE);
        break;
    case '}':
        addToken(TokenType::RIGHT_BRACE);
        break;
    case ',':
        addToken(TokenType::COMMA);
        break;
    case '-':
        addToken(TokenType::MINUS);
        break;
    case '+':
        addToken(TokenType::PLUS);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;

    case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;

    case '/': {
        // If comment
        if (match('/')) {
            // Consume char until new line
            while (peek() != '\n' && !isatEnd())
                advance();
        } else {
            addToken(TokenType::SLASH);
        }
        break;
    }

    case ' ':
        break;

    case '\n':
        incrementLine();
        break;

    case '"':
        string();
        break;

    default:
        if (std::isdigit(c)) {
            number();
        } else if (isAlpha(c) || c == '_') {
            identifier();
        } else {
            // error
        }
        break;
    }
}

void Lexer::addToken(TokenType type) {
    m_tokens.emplace_back(type, getLexeme(), m_line, m_column);
}

void Lexer::addToken(TokenType type, LiteralValue& literal) {

    m_tokens.emplace_back(type, getLexeme(), m_line, m_column, std::move(literal));
}

void Lexer::number() {
    while (std::isdigit(peek()))
        advance();

    // Decimal number (float)
    if (peek() == '.' && std::isdigit(peekNext())) {
        // consume '.'
        advance();
        while (std::isdigit(peek()))
            advance();

        LiteralValue value = std::stof(getLexeme());
        addToken(TokenType::FLOAT_LITERAL, value);
        return;
    }
    LiteralValue value = std::stoi(getLexeme());
    addToken(TokenType::INTEGER_LITERAL, value);
}

void Lexer::identifier() {
    while (isAlphanumeric(peek()) || peek() == '_')
        advance();

    auto identifier = getLexeme();

    auto type = KEYWORDS.find(identifier);
    if (type == KEYWORDS.end())
        m_tokens.emplace_back(TokenType::IDENTIFIER, std::move(identifier), m_line, m_column);
    else
        m_tokens.emplace_back(type->second, m_line, m_column);
}

std::string Lexer::getLexeme() {
    std::string lexeme{m_source.substr(m_start, m_current - m_start)};
    return lexeme;
}

bool Lexer::match(char expected) {
    if (peek() != expected || isatEnd())
        return false;

    advance();
    return true;
}

size_t Lexer::incrementLine() {
    m_column = 1;
    return m_line++;
}

bool Lexer::isatEnd() const {
    return m_current >= m_source.length();
}

char Lexer::peek() const {
    if (isatEnd())
        return '\0';
    return m_source.at(m_current);
}

char Lexer::peekNext() const {
    if (m_current + 1 >= m_source.length())
        return '\0';
    return m_source.at(m_current + 1);
}

char Lexer::advance() {
    ++m_column;
    return m_source.at(m_current++);
}

bool Lexer::isAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool Lexer::isAlphanumeric(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}
