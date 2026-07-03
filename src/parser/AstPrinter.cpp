#include "AstPrinter.hpp"

#include <print>
#include <sstream>

void PrintAST(const ExprNodePtr& exprNode) {
    std::println("{}", std::visit(PrintExpr{}, exprNode));
}

std::string Printer::printExpr(const ExprNodePtr& exprNode) {
    return std::visit(PrintExpr{}, exprNode);
}

std::string Printer::printStmt(const StmtNodePtr& stmtNode) {
    return std::visit(PrintStmt{}, stmtNode);
}

std::string PrintExpr::operator()(const LiteralExprPtr& expr) const {

    if (const auto* p = std::get_if<int>(&expr->m_Value)) {
        return std::to_string(*p);
    }
    if (expr->m_Value.index() == 0) {
        return "None";
    }
    if (const auto* p = std::get_if<float>(&expr->m_Value)) {
        return std::to_string(*p);
    }
    return std::get<std::string>(expr->m_Value);
}

std::string PrintExpr::operator()(const BinaryExprPtr& expr) const {
    return parenthesize(expr->m_operation.lexeme, {&expr->m_leftNode, &expr->m_rightNode});
}

std::string PrintExpr::operator()(const IdentifierExprPtr& expr) const {
    return expr->m_Name.lexeme;
}

// (name expr...)
std::string
PrintExpr::parenthesize(const std::string& name,
                        const std::initializer_list<const ExprNodePtr*> exprNodes) const {

    std::stringstream s;
    s << '(' << name;
    for (const auto& expr : exprNodes) {
        s << " ";
        s << std::visit(PrintExpr{}, *expr);
    }
    s << ')';
    return s.str();
}

std::string PrintStmt::operator()(const ExprStmtPtr& stmt) const {
    return Printer::printExpr(stmt->m_Expr);
}

std::string PrintStmt::operator()(const ReturnStmtPtr& stmt) const {
    return std::format("RETURN: {}", Printer::printExpr(stmt->m_RetValue));
}

std::string PrintStmt::operator()(const IfStmtPtr& stmt) const {
    return std::format("IF\n\tCONDITION:{}\n{}", Printer::printExpr(stmt->m_Condition),
                       indent("BODY:", {&stmt->m_Body}));
}

std::string PrintStmt::operator()(const FunctionStmtPtr& stmt) const {
    return std::format("FUNCTION \"{}\"\n  {}", stmt->m_Identifier.lexeme,
                       indent("BODY", {&stmt->m_Body}));
}

std::string PrintStmt::indent(const std::string& name,
                              std::initializer_list<const StmtNodePtr*> stmtNodes) const {
    std::stringstream s;
    s << name;
    for (const auto* stmt : stmtNodes) {
        s << "\n  | ";
        s << std::visit(PrintStmt{}, *stmt);
    }
    s << '\n';
    return s.str();
}

int main() {
    auto Lnode = createAstNode<LiteralExpr>(4);
    auto L2 = createAstNode<LiteralExpr>(1);
    auto Rnode = createAstNode<LiteralExpr>(1);

    auto plus = Token(TokenType::PLUS, "+", 1, 1);
    auto star = Token(TokenType::STAR, "*", 1, 1);

    auto B = createAstNode<BinaryExpr>(std::move(Lnode), plus, std::move(Rnode));
    auto A = createAstNode<BinaryExpr>(std::move(L2), star, std::move(B));
    auto rt = createAstNode<ReturnStmt>(std::move(A));

    Token name = Token(TokenType::IDENTIFIER, "main", 1, 2);
    auto func = createAstNode<FunctionStmt>(std::move(name), std::move(rt));
    std::println("{}", Printer::printStmt(func));
}