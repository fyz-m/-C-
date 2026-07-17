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
    return std::format("{} = {} {} {}",
                       printIRvariable(node->Result),
                       printIRvariable(node->Src1),
                       printOperation(node->Op),
                       printIRvariable(node->Src2));
}

// Result = Op Src1
std::string Printer::operator()(const IR::UnaryNodePtr& node) const {
    return std::format("{} = {} {}",
                       printIRvariable(node->Result),
                       printOperation(node->Op),
                       printIRvariable(node->Src1));
}

// var = vreg
std::string
Printer::operator()(const IR::AssignmentNodePtr& node) const {
    return std::format(
        "{} = {}", node->VarName, printIRvariable(node->Src1));
}

// vreg = var
std::string
Printer::operator()(const IR::AssignToVregPtr& node) const {
    return std::format(
        "{} = {}", printIRvariable(node->Result), node->VarName);
}

// ret Val
std::string Printer::operator()(const IR::ReturnNodePtr& node) const {
    return std::format("ret {}", printIRvariable(node->ReturnVal));
}

// Regular reg: tx
// Floating point: tfpx
// where x == ID of reg
std::string Printer::printVreg(const VirtualRegister& reg) {

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
    case NEG:
        return "-";
    case MULT:
        return "*";
    case DIV:
        return "/";
    case CMPLMNT:
        return "~";
    }
}
} // namespace IR