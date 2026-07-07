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

INSTANTIATE_TEST_SUITE_P(
    Parser, 
    ParseExprTest,
                        
    ::testing::Values 
    (
        // Basic precedence
        ExprTestCase("addition", "1 + 2", "(+ 1 2)"),
        ExprTestCase("add_mult", "1 + 2 * 3", "(+ 1 (* 2 3))"),
        ExprTestCase("mult_sub", "3 * 4 - 10", "(- (* 3 4) 10)"),
        ExprTestCase("div_before_add", "1 + 8 / 4", "(+ 1 (/ 8 4))"),

        // Associativity 
        ExprTestCase("addition_is_left_associative",
                        "1 + 2 + 3 + 4", 
                        "(+ (+ (+ 1 2) 3) 4)"),
        ExprTestCase("addition_and_subtraction_is_left_associative",
                        "1 - 2 + 3 - 4", 
                        "(- (+ (- 1 2) 3) 4)"),
        ExprTestCase("multiplication_is_left_associative",
                        "1 * 2 * 3 * 4", 
                        "(* (* (* 1 2) 3) 4)"),
        ExprTestCase("division_is_left_associative",
                        "8 / 4 / 2",
                        "(/ (/ 8 4) 2)"),

        // Grouping / parentheses
        ExprTestCase("simple_parens", "(1 + 2)", "(+ 1 2)"),
        ExprTestCase("parens_override_precedence",
                        "(1 + 2) * 3",
                        "(* (+ 1 2) 3)"),
        ExprTestCase("nested_parens",
                        "((1 + 2) * (3 - 4))",
                        "(* (+ 1 2) (- 3 4))"),
        ExprTestCase("redundant_parens", "(((5)))", "5"),

        // Unary operators
        ExprTestCase("unary_negation", "-5", "(- 5)"),

        ExprTestCase("unary_negation_in_expr",
                        "-5 + 3",
                        "(+ (- 5) 3)"),

        ExprTestCase("unary_negation_in_expr_2",
                        "10 + -3 - -4",
                        "(- (+ 10 (- 3)) (- 4))"),

        ExprTestCase("unary_binds_tighter_than_mult",
                        "-2 * 3",
                        "(* (- 2) 3)"),

        ExprTestCase("unary_binds_tighter_than_mult_2",
                        "-2 * -3 / -4",
                        "(/ (* (- 2) (- 3)) (- 4))"),

        ExprTestCase("decrement_unary", "--5", "(-- 5)"),
        ExprTestCase("tilde_unary", "-~var", "(- (~ var))"),
        ExprTestCase("decrement_and_negation_unary", "---5", "(-- (- 5))"),
        ExprTestCase("negate_then_decrement_unary", "-(--5)", "(- (-- 5))"),

        ExprTestCase{"assignment", "x = 4", "(= x 4)"},
        ExprTestCase{"assignment_2", "a = 1 + 2 * (3 - 4) / 5 / 3", "(= a (+ 1 (/ (/ (* 2 (- 3 4)) 5) 3)))"},
        ExprTestCase{"assignment_is_right_associative", "a=b=c+d", "(= a (= b (+ c d)))"},
        // ExprTestCase("logical_not", "!1", "(! 1)"),

        // // Comparisons
        // ExprTestCase("less_than", "1 < 2", "(< 1 2)"),
        // ExprTestCase("comparison_lower_precedence_than_add",
        //                 "1 + 2 < 4",
        //                 "(< (+ 1 2) 4)"),
        // ExprTestCase("equality", "1 == 2", "(== 1 2)"),
        // ExprTestCase("not_equal", "1 != 2", "(!= 1 2)"),
        // ExprTestCase("equality_lower_than_comparison",
        //                 "1 < 2 == 3 < 4",
        //                 "(== (< 1 2) (< 3 4))"),

        // // Logical operators
        // ExprTestCase("logical_and", "1 && 0", "(&& 1 0)"),
        // ExprTestCase("logical_or", "1 || 0", "(|| 1 0)"),
        // ExprTestCase("and_binds_tighter_than_or",
        //                 "1 || 0 && 1",
        //                 "(|| 1 (&& 0 1))"),
        // ExprTestCase("comparison_binds_tighter_than_logical",
        //                 "1 < 2 && 3 < 4",
        //                 "(&& (< 1 2) (< 3 4))"),

        // // Mixed / stress cases
        // ExprTestCase("full_precedence_stack",
        //                 "1 + 2 * 3 < 4 * 5 && 6 == 6",
        //                 "(&& (< (+ 1 (* 2 3)) (* 4 5)) (== 6 6))"),

        ExprTestCase("deeply_nested_arithmetic",
                        "1 + 2 * (3 - 4) / 5",
                        "(+ 1 (/ (* 2 (- 3 4)) 5))"),
        ExprTestCase{"int_literal_only", "4505", "4505"},
        ExprTestCase{"float_literal_only", "3.14", "3.140000"}
    ),

        [](const ::testing::TestParamInfo<ExprTestCase>& info) {
            return info.param.test_name;
            }
);

// NOLINTEND