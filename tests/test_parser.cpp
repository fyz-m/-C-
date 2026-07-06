#include "../src/lexer/lexer.hpp"
#include "../src/parser/AstPrinter.hpp"
#include "../src/parser/parser.hpp"

#include <gtest/gtest.h>

// NOLINTBEGIN
struct ExprTestCase {
    std::string test_name;
    std::string input;
    std::string expected_output;
};

class ParseExprTest : public ::testing::TestWithParam<ExprTestCase> {};

TEST_P(ParseExprTest, expression_test) {

    ExprTestCase tc = GetParam();

    Lexer lexer = Lexer(tc.input);
    auto tokens = lexer.Tokenize().value();
    Parser parser = Parser(tokens);
    auto output = AST::Printer::printExpr(parser.parseExpr());

    EXPECT_EQ(output, tc.expected_output);
}

INSTANTIATE_TEST_SUITE_P(Parser, ParseExprTest,
                        
::testing::Values(
    ExprTestCase("addition", "1 + 2","(+ 1 2)"),
    ExprTestCase("add_mult", "1 + 2 * 3","(+ 1 (* 2 3))"),
    ExprTestCase("mult_sub", "3 * 4 - 10","(- (* 3 4) 10)"),

    ExprTestCase("addition_is_right_associative",
                     "1 + 2 + 3 + 4", 
           "(+ (+ (+ 1 2) 3) 4)"),

    ExprTestCase("addition_and_subtraction_is_right_associative",
                     "1 - 2 + 3 - 4", 
           "(- (+ (- 1 2) 3) 4)"),

    ExprTestCase("multiplication_is_right_associative",
                     "1 * 2 * 3 * 4", 
           "(* (* (* 1 2) 3) 4)"),

    ExprTestCase("add_mult2", "1 + 2 * 3", "(+ 1 (* 2 3))"),
    ExprTestCase("add_mult3", "1 + 2 * 3", "(+ 1 (* 2 3))"),
    ExprTestCase("add_mult4", "1 + 2 * 3", "(+ 1 (* 2 3))"),
    ExprTestCase("add_mult5", "1 + 2 * 3", "(+ 1 (* 2 3))"),
    ExprTestCase("add_mult6", "1 + 2 * 3", "(+ 1 (* 2 3))")),

    [](const ::testing::TestParamInfo<ExprTestCase>& info) {
        return info.param.test_name;
        }
);

// NOLINTEND