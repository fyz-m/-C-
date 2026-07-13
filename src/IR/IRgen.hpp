#pragma once
#include "IRnodes.hpp"
#include "createIRnode.hpp"
#include "lexer/Token.hpp"

#include <vector>

namespace IR {

class Generator {

  private:
    std::vector<IRnode> m_Nodes;
    size_t m_CurrRegister{};
    // Floating point register count
    size_t m_CurrRegisterFP{};

  public:
    // Load an integer literal into a register
    VirtualRegister loadIntToReg(int integer);

  private:
    // Wrapper for createASTnode that also pushes the node onto
    // the Nodes vector
    template <IRnodeType T, typename... Args>
    void emit(Args... args) {
        m_Nodes.emplace_back(createIRnode<T>(std::forward<Args>(args)...));
    }

    VirtualRegister getRegister();

    VirtualRegister getRegisterFP();

    // Convert operator into its corresponding OPERATION in the TAC IR
    // different method for unary because same operator can translate to different operation
    // E.g '-' in binary is subtraction wheras is it negation in unary
    static constexpr OPERATION getBinaryOp(TokenType _operator) {
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
            throw std::runtime_error("Invalid operator");
        }
    }

    static constexpr OPERATION getUnaryOp(TokenType _operator) {
        switch (_operator) {
        case TokenType::MINUS:
            return OPERATION::CMPLMNT;

        default:
            // unreachable
            throw std::runtime_error("Invalid operator");
        }
    }
};

} // namespace IR