#include <iostream>
#include <chrono>
#include "MIPS.h"

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cout << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }
    MIPS vm;

    if (vm.loadProgram(argv[1]))
    {
        std::cerr << "An error occurred while loading the program: " << vm.getError() << std::endl;
        return EXIT_FAILURE;
    }
    std::clock_t start;
    std::clock_t end;
    start = std::clock();
    vm.run();
    end = std::clock() - start;
    if (vm.getError().length() != 0)
    { std::cerr << vm.getError() << std::endl; }
    std::cout << "Execution finished in " << end / (double) (CLOCKS_PER_SEC / 1000) << " ms" << std::endl;
}
