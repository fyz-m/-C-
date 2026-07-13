#pragma once
#include "IRnodes.hpp"
#include "lexer/Token.hpp"

#include <vector>

namespace IR {

class Generator {

  private:
    std::vector<IRnode> m_Nodes;
    size_t m_CurrRegister;

  public:
    // These functions emit a quadruple, returning the destination reg

    // Load an integer literal into a register
    size_t loadIntToReg(int integer);

  private:
    // Push a quadruple onto the list, returns dst (last used register)
    size_t emit();

    size_t nextRegister();

    // Convert operator into its corresponding OPERATION in the TAC IR
    // different method for unary because same operator can translate to different operation
    // E.g '-' in binary is subtraction wheras is it negation in unary
    constexpr OPERATION getOpBinary(TokenType _operator) {
        switch (_operator) {
        case TokenType::PLUS:
            return OPERATION::ADD;
        case TokenType::MINUS:
            return OPERATION::SUB;
        case TokenType::STAR:
            return OPERATION::MULT;
        case TokenType::SLASH:
            return OPERATION::DIV;

        default:
            // unreachable
        }
    }

    constexpr OPERATION getOpUnary(TokenType _operator) {
        switch (_operator) {
        case TokenType::MINUS:
            return OPERATION::CMPLMNT;

        default:
            // unreachable
        }
    }
};

} // namespace IR