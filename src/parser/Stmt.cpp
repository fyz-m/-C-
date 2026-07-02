#include "Stmt.hpp"

ReturnStmt::ReturnStmt(ExprNodePtr value)
    : m_RetValue{std::move(value)} {}

IfStmt::IfStmt(ExprNodePtr condition, StmtNodePtr body)
    : m_Condition{std::move(condition)}, m_Body{std::move(body)} {}