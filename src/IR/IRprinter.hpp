#pragma once
#include "IR/IRnodes.hpp"

namespace IR {

class Printer {

  public:
    static std::string printIR(std::span<IRnode> nodes);

    std::string operator()(const IR::BinaryNodePtr& node) const;
    std::string operator()(const IR::UnaryNodePtr& node) const;
    std::string operator()(const IR::AssignmentNodePtr& node) const;
    std::string operator()(const IR::AssignToVregPtr& node) const;
    std::string operator()(const IR::ReturnNodePtr& node) const;

    static std::string printIRoperand(IR::Operand operand);

  private:
    std::string printOperation(IR::OPERATION op) const;
};

} // namespace IR