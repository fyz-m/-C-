#include "driver.hpp"

int main(int argc, char* argv[]) {

    auto compiler_args = ParseArgs(argc, argv);

    if (compiler_args.InvalidArgs)
        return 1;
    run(compiler_args);
}
