#pragma once
#include "emit.hpp"

#include <optional>
#include <vector>

namespace IR {

enum class OPCODE : std::uint8_t { ADD, SUB, DIV, MULT, LI, NEG, RET };

struct Quadruple {
    std::optional<size_t> Src1; // source operands
    std::optional<size_t> Src2;
    OPCODE Op;
    std::optional<size_t> Dst; // destination of the result of: Src1 Op Src2

    Quadruple(std::optional<size_t> dst, OPCODE op, std::optional<size_t> src1,
              std::optional<size_t> src2)
        : Src1{src1}, Src2{src2}, Op{op}, Dst{dst} {}

    // For testing
    bool operator==(const Quadruple& other) const {
        if (this->Op != other.Op)
            return false;
        if (this->Src1 != other.Src1)
            return false;
        if (this->Src2 != other.Src2)
            return false;
        if (this->Dst != other.Dst)
            return false;

        return true;
    }
};

class Generator {
    friend struct EmitExpr;
    friend struct EmitStmt;

  private:
    std::vector<Quadruple> m_QuadList;
    size_t m_CurrRegister;

  public:
    size_t emitStmt(const StmtNodePtr& stmt);

    // emit an expression, returning the register that contains the value of the expression
    // evaluated
    size_t emitExpr(const ExprNodePtr& expr);

    // These functions emit a quadruple, returning the destination reg

    // Load an integer literal into a register
    size_t loadIntToReg(int integer);

  private:
    // Push a quadruple onto the list, returns dst (last used register)
    size_t emit(size_t dest, OPCODE op, std::optional<size_t> src1 = std::nullopt,
                std::optional<size_t> src2 = std::nullopt);

    void emit(OPCODE op, std::optional<size_t> src1 = std::nullopt,
              std::optional<size_t> src2 = std::nullopt);

    size_t nextRegister();

    // Convert operator into its corresponding opcode in the TAC IR
    // different method for unary because same operator can translate to different operation
    // E.g '-' in binary is subtraction wheras is it negation in unary
    constexpr OPCODE getOpcodeBinary(TokenType _operator) {
        switch (_operator) {
        case TokenType::PLUS:
            return OPCODE::ADD;
        case TokenType::MINUS:
            return OPCODE::SUB;
        case TokenType::STAR:
            return OPCODE::MULT;
        case TokenType::SLASH:
            return OPCODE::DIV;

        default:
            // unreachable
        }
    }

    constexpr OPCODE getOpcodeUnary(TokenType _operator) {
        switch (_operator) {
        case TokenType::MINUS:
            return OPCODE::NEG;

        default:
            // unreachable
        }
    }
};

} // namespace IR