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
                       printIRoperand(node->Result),
                       printIRoperand(node->Src1),
                       printOperation(node->Op),
                       printIRoperand(node->Src2));
}

// Result = Op Src1
std::string Printer::operator()(const IR::UnaryNodePtr& node) const {
    return std::format("{} = {} {}",
                       printIRoperand(node->Result),
                       printOperation(node->Op),
                       printIRoperand(node->Src1));
}

// var = vreg
std::string
Printer::operator()(const IR::AssignmentNodePtr& node) const {
    return std::format(
        "{} = {}", node->VarName, printIRoperand(node->Src1));
}

// vreg = var
std::string
Printer::operator()(const IR::AssignToVregPtr& node) const {
    return std::format(
        "{} = {}", printIRoperand(node->Result), node->VarName);
}

// ret Val
std::string Printer::operator()(const IR::ReturnNodePtr& node) const {
    return std::format("ret {}", printIRoperand(node->ReturnVal));
}

template <class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};
std::string Printer::printIRoperand(IR::Operand operand) {
    return std::visit(
        Overloaded{[&](const std::string& s) { return s; },
                   [&](int i) { return std::to_string(i); }},
        operand);
};

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