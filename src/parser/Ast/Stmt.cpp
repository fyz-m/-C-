#include "Stmt.hpp"

ReturnStmt::ReturnStmt(std::optional<ExprNodePtr> value)
    : m_RetValue{std::move(value)} {}

IfStmt::IfStmt(ExprNodePtr condition, StmtNodePtr body)
    : m_Condition{std::move(condition)}, m_Body{std::move(body)} {}

FunctionStmt::FunctionStmt(Token identifier, BlockStmtPtr body)
    : m_Identifier{std::move(identifier)}, m_Body{std::move(body)} {}

VarDeclarationStmt::VarDeclarationStmt(Token name, ExprNodePtr initializer)
    : m_VarName{std::move(name)}, m_Initializer{std::move(initializer)} {}

BlockStmt::BlockStmt(std::vector<StmtNodePtr> statements)
    : m_statements{std::move(statements)} {}