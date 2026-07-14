#include "IR/IRgen.hpp"
#include "IR/IRprinter.hpp"
#include "parser/parser.hpp"
#include "lexer/lexer.hpp"
#include <gtest/gtest.h>

// NOLINTBEGIN


struct IRTestCase {
    std::string test_name;
    std::string input;
    std::string expected;
};

class GenIRtest : public ::testing::TestWithParam<IRTestCase> {};

TEST_P(GenIRtest, expression) {
    IRTestCase tc = GetParam();
 
    Lexer lexer = Lexer(tc.input);
    auto tokens = lexer.Tokenize().value();
 
    Parser parser = Parser(tokens);
    auto nodes = parser.Parse();

    IR::Generator generator = IR::Generator(nodes.value());
    auto output = IR::Printer::printIR(generator.generateIR());

    EXPECT_EQ(output, tc.expected);
}

INSTANTIATE_TEST_SUITE_P(
    IR, 
    GenIRtest,

    ::testing::Values
    (
        IRTestCase("addition", "1 + 2;", "t0 = 1 + 2\n"),
        IRTestCase("addition_multiple_operands", "1 + 2 + 3 + 4;", 
                    "t0 = 1 + 2\n" 
                              "t1 = t0 + 3\n"
                              "t2 = t1 + 4\n"
                 ),
        IRTestCase("precedence", "1 * -2 / 3 + 4;", 
                    "t0 = ~ 2\n" 
                              "t1 = 1 * t0\n"
                              "t2 = t1 / 3\n"
                              "t3 = t2 + 4\n"
                 ),
        IRTestCase("precedence_grouping", "1 * -2 / (3 + -4);", 
                    "t0 = ~ 2\n" 
                              "t1 = 1 * t0\n"
                              "t2 = ~ 4\n"
                              "t3 = 3 + t2\n"
                              "t4 = t1 / t3\n"
                 ),

        // IRTestCase("addition1", "1 + 2", "t0 = 1 + 2"),
        // IRTestCase("addition2", "1 + 2", "t0 = 1 + 2"),
        // IRTestCase("addition3", "1 + 2", "t0 = 1 + 2"),
        // IRTestCase("addition4", "1 + 2", "t0 = 1 + 2"),
        // IRTestCase("addition5", "1 + 2", "t0 = 1 + 2"),
        // IRTestCase("addition6", "1 + 2", "t0 = 1 + 2"),
        // IRTestCase("addition7", "1 + 2", "t0 = 1 + 2"),
        IRTestCase("addition9", "1 + 2;", "t0 = 1 + 2\n")
    ),

    [](const ::testing::TestParamInfo<IRTestCase>& info) {
        return info.param.test_name;
    }
);


// NOLINTEND