#pragma once
#include "IR/IRnodes.hpp"
#include "parser/Ast/Stmt.hpp"

namespace IR {

class Printer {

  public:
    static std::string printIR(const std::vector<IRnode>& nodes);

  private:
    std::string operator()(const IR::BinaryNodePtr& expr) const;
    std::string operator()(const IR::UnaryNodePtr& expr) const;
    std::string operator()(const IR::AssignToVregNodePtr& expr) const;
};
} // namespace IR