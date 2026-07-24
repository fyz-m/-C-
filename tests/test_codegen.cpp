#include <gtest/gtest.h>
#include "../src/lexer/lexer.hpp"
#include "IR/IRgen.hpp"
#include "codegen/codegen.hpp"
#include "codegen/reg_alloc/register_allocator.hpp"
#include "codegen/code_emission.hpp"
#include "parser/parser.hpp"
#include "IR/IRprinter.hpp"
#include "../src/codegen/expand_pseudo/expand_pseudo.hpp"


std::string removeNewlines(std::string& str) {
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}

using namespace CODEGEN;

// NOLINTBEGIN
struct CodeGenTestCase {
    std::string test_name;
    std::string input;
    std::string expected_IR;
    std::string expected_asm;
};

class CodeGentest : public ::testing::TestWithParam<CodeGenTestCase> {};

TEST_P(CodeGentest, codegenFirstPassOnly) {
    CodeGenTestCase tc = GetParam();
 
    auto ir = std::move(IR::Generator(Parser(
        Lexer(tc.input).Tokenize().value()
    ).Parse().value()).generateIR());

    ASSERT_EQ(IR::Printer::printIR(ir), tc.expected_IR);
    auto cg = CodeGenerator(ir); 
    cg.generateRISCVassembly();
    auto& asm_nodes = cg.getInstructions();

    EXPECT_EQ(CodeEmitter(true).emitAsm(asm_nodes), tc.expected_asm);
}

INSTANTIATE_TEST_SUITE_P(
    cg, 
    CodeGentest,

    ::testing::Values
    (       
        CodeGenTestCase{"unary_negate_var", "-a;", "t.0 = - a\n", "neg t.0, a\n"},
        CodeGenTestCase{"unary_negate_imm", "-2;", "t.0 = - 2\n", "li t.0, 2\n" "neg t.0, t.0\n"},
        
        CodeGenTestCase{"unary_nested", "-~2;", "t.0 = ~ 2\n" "t.1 = - t.0\n", 
                                                                "li t.0, 2\n" "not t.0, t.0\n"
                                                                "neg t.1, t.0\n"},

        // CodeGenTestCase{"return_void", "return;", "ret\n", "ret\n"},
        CodeGenTestCase{"return_imm", "return 5;", "ret 5\n", "addi a0, zero, 5\n" "ret\n"}
    ),

    [](const ::testing::TestParamInfo<CodeGenTestCase>& info) {
        return info.param.test_name;
    }
);

struct IR_to_ASM_testcase {
    std::string testName;
    std::vector<IR::IRnode> ir_instructions;
    std::string expected_asm;

    IR_to_ASM_testcase(std::string tname, 
                       std::initializer_list<std::function<IR::IRnode()>> ir_gen_functions,
                       std::string expected_asm)

                       : testName{std::move(tname)}, 
                         expected_asm{std::move(expected_asm)}

                       {
                        ir_instructions.reserve(ir_gen_functions.size());
                        for (auto& function : ir_gen_functions)                
                            ir_instructions.push_back(function());
                       }
};
 
class IR_to_ASM_test : public ::testing::TestWithParam<std::shared_ptr<IR_to_ASM_testcase>> {};

TEST_P(IR_to_ASM_test, codegenStackAllocateOnly) {

    auto tc = GetParam();
    
    auto cg = CodeGenerator(tc->ir_instructions); 
    cg.generateRISCVassembly();
    CODEGEN::RegisterAllocator::varToMem(cg.getInstructions());

    auto actual_asm = CodeEmitter(true).emitAsm(cg.getInstructions());
    EXPECT_EQ(removeNewlines(actual_asm), tc->expected_asm);
}


INSTANTIATE_TEST_SUITE_P(
    var_to_stack,
    IR_to_ASM_test, 

    ::testing::Values(

         std::make_shared<IR_to_ASM_testcase>(
            "unary_neg",
            std::initializer_list<std::function<IR::IRnode()>>{
                [](){ return createIRnode<IR::UnaryNode>("a", IR::OPERATION::NEG, "a"); } // a = - a
            },
            "neg sp(0), sp(0)"
        )

    ),

    [](const ::testing::TestParamInfo<std::shared_ptr<IR_to_ASM_testcase>>& info) {
        return info.param->testName;
    }
);


struct PItestCase {
    std::string test_name;
    // Store the actual function to create an instruction because 
    // gtest copies the test case and RISCV::instruction variant 
    // is uncopyable (contains unique ptrs)
    std::function<RISCV::Instruction()> create_input_instruction;
    std::function<RISCV::Instruction()> create_expected_instruction;
};

class PseudoInstrutionTest : public ::testing::TestWithParam<PItestCase> {};

TEST_P(PseudoInstrutionTest, single_instruction) {
    auto& tc = GetParam();
    auto input = tc.create_input_instruction();
    auto expected = tc.create_expected_instruction();

    convertPseudoInstruction(input);

    EmitAsm emitter{false};
    EXPECT_EQ(std::visit(emitter, input), std::visit(emitter, expected));    
}

using namespace RISCV;
using namespace OPCODE;
using enum I_TYPE;
using enum R_TYPE;
using enum REGISTER;
INSTANTIATE_TEST_SUITE_P(
    expand,
    PseudoInstrutionTest,

    ::testing::Values(
        
        PItestCase{"LI_imm_less_than_12_bits",
             // li x1, 2
             [](){ return createInstruction<LI>(x1, 2); },
            // -> addi x1, zero, 2
             [](){ return createInstruction<Itype>(addi, x1, zero, 2); }
            },

        PItestCase{"LI_imm_equal_to_12_bits",
             // li x1, -2048
             [](){ return createInstruction<LI>(x1, 2048); },
            // -> addi x1, zero, -2048
             [](){ return createInstruction<Itype>(addi, x1, zero, 2048); }
            },


        PItestCase{"ret",
             // ret 
             [](){ return createInstruction<RET>(); },
            // -> jalr x0, ra, 0
             [](){ return createInstruction<Itype>(jalr, x0, ra, 0); }
            },

        PItestCase{"mv",
             // mv x1, x2
             [](){ return createInstruction<MV>(x1, x2); },
            // -> addi x1, x2, 0
             [](){ return createInstruction<Itype>(addi, x1, x2, 0); }
            },

        PItestCase{"not",
             // not x1, x2
             [](){ return createInstruction<NOT>(x1, x2); },
            // -> xori x1, x2, -1
             [](){ return createInstruction<Itype>(xori, x1, x2, -1); }
            },

        PItestCase{"neg",
             // neg x1, x2
             [](){ return createInstruction<NEG>(x1, x2); },
            // -> sub x1, zero, x2
             [](){ return createInstruction<Rtype>(sub, x1, zero, x2); }
            }
    ),

    [](const ::testing::TestParamInfo<PItestCase>& info) {
        return info.param.test_name;
    }
);





// NOLINTEND