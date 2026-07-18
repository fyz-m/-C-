#pragma once
#include "IRnodes.hpp"
#include "createIRnode.hpp"
#include "lexer/Token.hpp"
#include "parser/Ast/Stmt.hpp"

#include <vector>

namespace IR {
struct ExprVisitor;
class Generator {

    friend struct ExprVisitor;

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
        m_Nodes.push_back(
            createIRnode<T>(std::forward<Args>(args)...));
    }

    // Load an integer literal into a register
    Variable loadVarToReg(int integer);

    // Create a new variable
    // We need to make new variables so we can store the intermediate
    // result(s) of expressions with more than 2 operands, because 3AC
    // IR only allows a maximum of two operands
    Variable getTempVar(bool floating_point = false);

    // Convert operator into its corresponding OPERATION in the TAC IR
    // different method for unary because same operator can translate
    // to different operation E.g '-' in binary is subtraction wheras
    // is it negation in unary
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
        case TokenType::TILDE:
            return OPERATION::CMPLMNT;
        case TokenType::MINUS:
            return OPERATION::NEG;
        default:
            // unreachable
            throw std::runtime_error("Invalid operator");
        }
    }
};

// Visitors for AST nodes
// Translates a given AST node into a 3AC IR node(s)
struct ExprVisitor {

    Generator& Gen;

    // This is used by AssignmentExprPtr visitor to tell the callee
    // that its result should be stored in this name. Otherwise store
    // result in a new variable.
    // e.g Assignment:
    // a = 1 + 2 -> Resultname = "a" -> BinaryNode(a, 1, PLUS, 2)
    // Expr:
    // 1 + 2 -> BinaryNode(t.0, 1, PLUS, 2);
    std::optional<Variable> Resultname;

    ExprVisitor(Generator& generator)
        : Gen{generator} {}

    ExprVisitor(Generator& generator, Variable var)
        : Gen{generator}, Resultname{std::move(var)} {}

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
} // namespace IR