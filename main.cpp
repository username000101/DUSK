#include <iostream>

#include "Args.hpp"
#include "Macros.hpp"


int main(int argc, char** argv) {
    std::cout << DUSK_LOGO << std::endl << std::endl;
    args::process_args(argc, argv);
    return 0;
}
