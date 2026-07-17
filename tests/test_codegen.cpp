#include <gtest/gtest.h>
#include "../src/lexer/lexer.hpp"
#include "IR/IRgen.hpp"
#include "codegen/codegen.hpp"
#include "codegen/code_emission.hpp"
#include "parser/parser.hpp"
#include "IR/IRprinter.hpp"

// NOLINTBEGIN

using namespace CODEGEN;

struct CodeGenTestCase {
    std::string test_name;
    std::string input;
    std::string expected_IR;
    std::string expected_asm;
};

class CodeGentest : public ::testing::TestWithParam<CodeGenTestCase> {};

TEST_P(CodeGentest, cg) {
    CodeGenTestCase tc = GetParam();
 
    auto ir = std::move(IR::Generator(Parser(
        Lexer(tc.input).Tokenize().value()
    ).Parse().value()).generateIR());

    ASSERT_EQ(IR::Printer::printIR(ir), tc.expected_IR);

    auto asm_nodes = std::move(CodeGenerator(ir).generateRISCVassembly());

    EXPECT_EQ(CodeEmitter(true).emitAsm(asm_nodes), tc.expected_asm);
}

INSTANTIATE_TEST_SUITE_P(
    cg, 
    CodeGentest,

    ::testing::Values
    (       
        CodeGenTestCase{"unary_negate", "-2;", "t0 = - 2\n", "addi t0 "},
        CodeGenTestCase{"return_imm", "return 5;", "ret 5\n", "addi a0, zero, 5\nret"}
    ),

    [](const ::testing::TestParamInfo<CodeGenTestCase>& info) {
        return info.param.test_name;
    }
);


// NOLINTEND