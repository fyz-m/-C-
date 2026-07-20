cmake_minimum_required(VERSION 3.28.3)
project(RVC LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library
add_library(rvc_core
src/diagnostics/DiagnosticsEngine.cpp
  src/lexer/lexer.cpp
  src/parser/AstPrinter.cpp
  src/parser/parser.cpp
  src/parser/Ast/Expr.cpp
  src/parser/Ast/Stmt.cpp
  src/IR/IRprinter.cpp
  src/IR/IRnodes.cpp
  src/IR/IRgen.cpp
  src/codegen/codegen.cpp
  src/codegen/backend_driver.cpp
  src/codegen/code_emission.cpp
  src/codegen/expand_pseudo/expand_pseudo.cpp
  )

target_include_directories(rvc_core PUBLIC src)
target_compile_options(rvc_core PUBLIC -Wall -Wextra -Wpedantic -g3 -O0 -Wno-unused-parameter -Wnrvo)

# Main executable (entry point)
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE rvc_core)

# GoogleTest setup 
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test executable 
add_executable(unit_tests
  tests/test_parser.cpp
  tests/test_IR.cpp
  tests/test_codegen.cpp
)

target_link_libraries(unit_tests PRIVATE rvc_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)