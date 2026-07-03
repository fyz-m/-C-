#include "Stmt.hpp"

ReturnStmt::ReturnStmt(ExprNodePtr value)
    : m_RetValue{std::move(value)} {}

IfStmt::IfStmt(ExprNodePtr condition, StmtNodePtr body)
    : m_Condition{std::move(condition)}, m_Body{std::move(body)} {}

FunctionStmt::FunctionStmt(Token identifier, StmtNodePtr body)
    : m_Identifier{std::move(identifier)}, m_Body{std::move(body)} {}