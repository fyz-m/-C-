#pragma once
#include "parser/Ast/Expr.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace IR {
struct ConvertExpr;

enum class OPCODE : std::uint8_t { ADD, SUB, DIV, MULT, LI, NEG };

struct Quadruple {
    std::optional<size_t> Src1; // source operands
    std::optional<size_t> Src2;
    OPCODE Op;
    size_t Dst; // destination of the result of: Src1 Op Src2

    Quadruple(size_t dst, OPCODE op, std::optional<size_t> src1, std::optional<size_t> src2)
        : Src1{src1}, Src2{src2}, Op{op}, Dst{dst} {}
};

class Generator {
    friend struct ConvertExpr;

  private:
    std::vector<Quadruple> m_QuadList;
    size_t m_CurrRegister;

  public:
    // emit an expression, returning the register that contains the value of the expression
    // evaluated
    size_t emitExpr(const ExprNodePtr& expr);

    // These functions emit a quadruple, returning the destination reg

    // Load an integer literal into a register
    size_t loadIntToReg(int integer);

  private:
    // Push a quadruple onto the list, returns dst for convienience
    size_t emit(size_t dest, OPCODE op, std::optional<size_t> src1 = std::nullopt,
                std::optional<size_t> src2 = std::nullopt);

    // Convert operator into its corresponding opcode in the TAC IR
    // different method for unary because same operator can translate to different operation
    // E.g '-' in binary is subtraction wheras is it negation in unary
    constexpr OPCODE getOpcodeBinary(TokenType _operator);

    constexpr OPCODE getOpcodeUnary(TokenType _operator);

    size_t nextRegister();
};

struct ConvertExpr {
    Generator& gen;
    ConvertExpr(Generator& gen)
        : gen{gen} {}
    size_t operator()(const LiteralExprPtr& expr) const;
    size_t operator()(const BinaryExprPtr& expr) const;
    size_t operator()(const UnaryExprPtr& expr) const;
    size_t operator()(const IdentifierExprPtr& expr) const;
    size_t operator()(const AssignmentExprPtr& expr) const;
};
} // namespace IR