#include "IRprinter.hpp"

#include <sstream>

namespace IR {
std::string Printer::printIR(std::span<IRnode> nodes) {
    std::stringstream s;
    for (const auto& node : nodes)
        s << std::visit(IR::Printer{}, node) << '\n';
    return s.str();
}

// Result = Src1 Op Src2
std::string Printer::operator()(const IR::BinaryNodePtr& node) const {
    return std::format("{} = {} {} {}", printVreg(node->Result), printOperand(node->Src1),
                       printOperation(node->Op), printOperand(node->Src2));
}

// Result = Op Src1
std::string Printer::operator()(const IR::UnaryNodePtr& node) const {
    return std::format("{} = {} {}", printVreg(node->Result), printOperation(node->Op),
                       printOperand(node->Src1));
}

// Vreg = imm
std::string Printer::operator()(const IR::AssignToVregNodePtr& node) const {}

std::string Printer::printOperand(const Operand& operand) const {

    if (const auto* val = std::get_if<int>(&operand))
        return std::to_string(*val);

    if (const auto* reg = std::get_if<VirtualRegister>(&operand)) {
        return printVreg(*reg);
    }

    return " ";
}

// Regular reg: tx
// Floating point: tfpx
// where x == ID of reg
std::string Printer::printVreg(const VirtualRegister& reg) const {

    std::string type;

    if (reg.Type == IR::VREGTYPE::REGULAR)
        type = "t";
    else
        type = "tfp";

    return std::format("{}{}", type, reg.ID);
}

std::string Printer::printOperation(IR::OPERATION op) const {
    using enum OPERATION;

    switch (op) {
    case ADD:
        return "+";
    case SUB:
        return "-";
    case MULT:
        return "x";
    case DIV:
        return "÷";
    case CMPLMNT:
        return "~";
    }
}
} // namespace IR