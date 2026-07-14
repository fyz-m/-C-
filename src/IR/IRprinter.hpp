#pragma once
#include "IR/IRnodes.hpp"

namespace IR {

class Printer {

  public:
    static std::string printIR(std::span<IRnode> nodes);

    std::string operator()(const IR::BinaryNodePtr& node) const;
    std::string operator()(const IR::UnaryNodePtr& node) const;
    std::string operator()(const IR::AssignToVregNodePtr& node) const;

  private:
    std::string printOperation(IR::OPERATION op) const;
    std::string printOperand(const Operand& operand) const;
    std::string printVreg(const VirtualRegister& reg) const;
};
} // namespace IR