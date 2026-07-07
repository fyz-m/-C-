#include "Expr.hpp"

BinaryExpr::BinaryExpr(ExprNodePtr left, Token operation, ExprNodePtr right)
    : m_leftNode{std::move(left)},
      m_operation{std::move(operation)},
      m_rightNode{std::move(right)} {}

UnaryExpr::UnaryExpr(Token operation, ExprNodePtr expression)
    : m_operation{std::move(operation)}, m_expression{std::move(expression)} {}

LiteralExpr::LiteralExpr(LiteralValue value)
    : m_Value{std::move(value)} {}

IdentifierExpr::IdentifierExpr(Token name)
    : m_Name{std::move(name)} {}
