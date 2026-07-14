#pragma once
#include "IRnodes.hpp"
#include "createIRnode.hpp"
#include "lexer/Token.hpp"
#include "parser/Ast/Stmt.hpp"

#include <vector>

namespace IR {

class Generator {

  private:
    std::vector<IRnode> m_Nodes;
    std::span<const StmtNodePtr> m_AST;

    // Track virtual registers
    size_t m_CurrRegister{};
    size_t m_CurrRegisterFP{};

  public:
    Generator(std::span<const StmtNodePtr> ast);

    std::vector<IRnode>& generateIR();

    // Wrapper for createASTnode that also pushes the node onto
    // the Nodes vector
    template <IRnodeType T, typename... Args>
    void emit(Args... args) {
        m_Nodes.push_back(createIRnode<T>(std::forward<Args>(args)...));
    }

    // Load an integer literal into a register
    VirtualRegister loadIntToReg(int integer);

    VirtualRegister getRegister(VREGTYPE type = VREGTYPE::REGULAR);

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

    // Visitors for AST nodes
    // Translates a given AST node into a 3AC IR node
    struct ExprVisitor {
        Generator& Gen;
        ExprVisitor(Generator& generator)
            : Gen{generator} {}

        Operand operator()(const LiteralExprPtr& expr) const;
        Operand operator()(const BinaryExprPtr& expr) const;
        Operand operator()(const UnaryExprPtr& expr) const;
        Operand operator()(const IdentifierExprPtr& expr) const;
        Operand operator()(const AssignmentExprPtr& expr) const;
    };

    struct StmtVisitor {
        Generator& Gen;
        StmtVisitor(Generator& generator)
            : Gen{generator} {}

        void operator()(const ExprStmtPtr& stmt) const;
        void operator()(const BlockStmtPtr& stmt) const;
        void operator()(const ReturnStmtPtr& stmt) const;
        void operator()(const IfStmtPtr& stmt) const;
        void operator()(const FunctionStmtPtr& stmt) const;
        void operator()(const VarDeclarationStmtPtr& stmt) const;
    };

  private:
    ExprVisitor m_ExprVisitor;
    StmtVisitor m_StmtVisitor;
};

} // namespace IR