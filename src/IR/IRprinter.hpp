#pragma once
#include "IR/IRnodes.hpp"

template <typename T>
concept IRVAR = std::same_as<T, IR::VirtualRegister> ||
                std::same_as<T, IR::Operand>;

namespace IR {

class Printer {

  public:
    static std::string printIR(std::span<IRnode> nodes);

    std::string operator()(const IR::BinaryNodePtr& node) const;
    std::string operator()(const IR::UnaryNodePtr& node) const;
    std::string operator()(const IR::AssignmentNodePtr& node) const;
    std::string operator()(const IR::AssignToVregPtr& node) const;
    std::string operator()(const IR::ReturnNodePtr& node) const;

    static std::string printVreg(const VirtualRegister& reg);

  private:
    std::string printOperation(IR::OPERATION op) const;

    // An IR variable is <string, VirtualRegister, int>
    template <IRVAR T>
    std::string printIRvariable(T operand) const {

        if constexpr (std::same_as<T, VirtualRegister>)
            return printVreg(operand);

        if constexpr (std::same_as<T, Operand>) {

            if (const auto* val = std::get_if<std::string>(&operand))
                return *val;

            if (const auto* reg =
                    std::get_if<VirtualRegister>(&operand)) {
                return printVreg(*reg);
            }

            return std::to_string(std::get<int>(operand));
        }
    }
};
} // namespace IR