#include "lexer.hpp"

#include "../diagnostics/DiagnosticsEngine.hpp"

Lexer::Lexer(std::string_view source)
    : m_source{source} {
    m_tokens.reserve(1000 * sizeof(Token));
}

std::expected<std::vector<Token>, bool> Lexer::Tokenize() {

    while (!isatEnd()) {
        m_start = m_current;
        m_column_start = m_column_end;
        scanToken();
    }

    // Add last line
    addLineToDE();

    if (m_hadError)
        return std::unexpected(false);

    m_tokens.emplace_back(TokenType::END_OF_FILE, Location());
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
    case '+':
        addToken(TokenType::PLUS);
        break;
    case '~':
        addToken(TokenType::TILDE);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;

    case '-':
        addToken(match('-') ? TokenType::MINUS_MINUS
                            : TokenType::MINUS);
        break;
    case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL
                            : TokenType::BANG);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL
                            : TokenType::EQUAL);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL
                            : TokenType::LESS);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL
                            : TokenType::GREATER);
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

        // case '"':
        //     string();
        //     break;

    default:
        if (std::isdigit(c)) {
            number();
        } else if (std::isalpha(static_cast<unsigned char>(c)) ||
                   c == '_') {
            identifier();
        } else {
            // TODO:
            // Report multiple unknown chars as one Error
            // so we don't flood the terminal with the
            // same error
            m_hadError = true;
            Diagnostics::DiagnosticsEngine::report(
                "Unexpected character.", m_line, m_column_start);
        }
        break;
    }
}

void Lexer::addToken(TokenType type) {
    m_tokens.emplace_back(type, getLexeme(), getLocation());
}

void Lexer::addToken(TokenType type, LiteralValue& literal) {

    m_tokens.emplace_back(
        type, getLexeme(), getLocation(), std::move(literal));
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
    while (std::isalnum(peek()) || peek() == '_')
        advance();

    auto identifier = getLexeme();

    auto type = m_KEYWORDS.find(identifier);
    if (type == m_KEYWORDS.end())
        m_tokens.emplace_back(TokenType::IDENTIFIER,
                              std::move(identifier),
                              getLocation());
    else
        m_tokens.emplace_back(type->second, getLocation());
}

std::string Lexer::getLexeme() const {
    return std::string{m_source.substr(m_start, m_current - m_start)};
}

Location Lexer::getLocation() const {
    return Location(m_line, m_column_start, m_column_end);
}

bool Lexer::match(char expected) {
    if (peek() != expected || isatEnd())
        return false;

    advance();
    return true;
}

size_t Lexer::incrementLine() {
    addLineToDE();
    m_column_start = 1;
    m_column_end = 1;
    return m_line++;
}

void Lexer::addLineToDE() const {
    static size_t line_start = 0;
    static size_t line_end = 0;

    line_end = m_current;
    Diagnostics::DiagnosticsEngine::addLine(line_start, line_end);
    // start of next line
    line_start = m_current;
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
    ++m_column_end;
    return m_source.at(m_current++);
}
