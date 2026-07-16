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
        IRTestCase("binary", "1 + 2;", "t0 = 1 + 2\n"),
        IRTestCase("binary_var", "a + b;", "t0 = a + b\n"),
        IRTestCase("binary_multiple_operands", "1 + 2 + 3 + 4;", 
                    "t0 = 1 + 2\n" 
                    "t1 = t0 + 3\n"
                    "t2 = t1 + 4\n"
                 ),
        IRTestCase("unary", "-2;", "t0 = ~ 2\n"),
        IRTestCase("unary_var", "-2;", "t0 = ~ 2\n"),

        IRTestCase("precedence", "1 * -2 / 3 + 4;", 
                    "t0 = ~ 2\n" 
                    "t1 = 1 * t0\n"
                    "t2 = t1 / 3\n"
                    "t3 = t2 + 4\n"
                 ),
        IRTestCase("precedence_grouping", "1 * -x / (y + -4);", 
                    "t0 = ~ x\n" 
                    "t1 = 1 * t0\n"
                    "t2 = ~ 4\n"
                    "t3 = y + t2\n"
                    "t4 = t1 / t3\n"
                 ),
        IRTestCase("assignment", "x = 4;", "x = 4\n"),
        IRTestCase("assignment_2", "x = 4; y = 3 * 4; x = y;", 
                    "x = 4\n"
                    "t0 = 3 * 4\n"
                    "y = t0\n"
                    "x = y\n"
                ),
        IRTestCase("chained_assignment", "a = 3; a = b = c = 3;", 
                    "x = 4\n"
                    "t0 = 3 * 4\n"
                    "y = t0\n"
                    "x = y\n"
                ),

        IRTestCase("return", "return 7;", "ret 7\n"),
        IRTestCase("return_var", "return a;", "ret a\n"),
        IRTestCase("return_expr", "return  -4 * 2;", 
                    "t0 = ~ 4\n"
                    "t1 = t0 * 2\n"
                    "ret t1\n"
                ),


        IRTestCase("addition9", "1 + 2;", "t0 = 1 + 2\n")
    ),

    [](const ::testing::TestParamInfo<IRTestCase>& info) {
        return info.param.test_name;
    }
);


// NOLINTEND