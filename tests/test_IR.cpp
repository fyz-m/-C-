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
        IRTestCase("binary", "1 + 2;", "t.0 = 1 + 2\n"),
        IRTestCase("binary_var", "a + b;", "t.0 = a + b\n"),
        IRTestCase("binary_multiple_operands", "1 + 2 + 3 + 4;", 
                    "t.0 = 1 + 2\n" 
                    "t.1 = t.0 + 3\n"
                    "t.2 = t.1 + 4\n"
                 ),
        IRTestCase("unary_complement", "~2;", "t.0 = ~ 2\n"),
        IRTestCase("unary_negation", "-2;", "t.0 = - 2\n"),
        IRTestCase("unary_var", "-2;", "t.0 = - 2\n"),

        IRTestCase("unary_chained", "-~2;", 
            "t.0 = ~ 2\n" 
            "t.1 = - t.0\n"),

        IRTestCase("precedence", "1 * -2 / 3 + 4;", 
                    "t.0 = - 2\n" 
                    "t.1 = 1 * t.0\n"
                    "t.2 = t.1 / 3\n"
                    "t.3 = t.2 + 4\n"
                 ),
        IRTestCase("precedence_grouping", "1 * -x / (y + -4);", 
                    "t.0 = - x\n" 
                    "t.1 = 1 * t.0\n"
                    "t.2 = - 4\n"
                    "t.3 = y + t.2\n"
                    "t.4 = t.1 / t.3\n"
                 ),
        IRTestCase("assignment", "x = 4;", "x = 4\n"),
        IRTestCase("assignment_2", "x = 4; y = 3 * 4; x = y;", 
                    "x = 4\n"
                    "y = 3 * 4\n"
                    "x = y\n"
                ),
        // IRTestCase("assignment_aa", "x = 4; y = 3 * 4; x = y;", 
        //             "x = 4\n"
        //             "y = 3 * 4\n"
        //             "x = y\n"
        //         ), 
        IRTestCase("assignment_chained",
                       "a = 3;"
                    "a = b = c = 3;",

                    "a = 3\n"
                    "c = 3\n"
                    "b = c\n"
                    "a = b\n"
                ),
        IRTestCase("assignment_complex",
                       "c = 1 + 2 * (19 - 4) / 5;"
                    "a = c = c + 3;"
                    ,

                    "t.0 = 19 - 4\n"
                    "t.1 = 2 * t.0\n"
                    "t.2 = t.1 / 5\n"
                    "c = 1 + t.2\n"

                    "c = c + 3\n"
                    "a = c\n"
                ),

        IRTestCase("return", "return 7;", "ret 7\n"),
        IRTestCase("return_var", "return a;", "ret a\n"),
        IRTestCase("return_expr", "return  -4 * 2;", 
                    "t.0 = - 4\n"
                    "t.1 = t.0 * 2\n"
                    "ret t.1\n"
                ),


        IRTestCase("addition9", "1 + 2;", "t.0 = 1 + 2\n")
    ),

    [](const ::testing::TestParamInfo<IRTestCase>& info) {
        return info.param.test_name;
    }
);


// NOLINTEND