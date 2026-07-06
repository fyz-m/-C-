cmake_minimum_required(VERSION 3.28.3)

project(RVC LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main src/main.cpp 
                      src/lexer/lexer.cpp
                      src/parser/AstPrinter.cpp
                      src/parser/parser.cpp
                      src/parser/Ast/Expr.cpp
                      src/parser/Ast/Stmt.cpp
                      src/diagnostics/DiagnosticsEngine.cpp
)

target_compile_options(main  PRIVATE -Wall -Wextra -Wpedantic  -g3 -O0 -Wno-unused-parameter)
# target_link_options(main PRIVATE -fsanitize=address,undefined)