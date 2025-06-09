#include "Args.hpp"
#include "FS.hpp"

#include "Macros.hpp"

#include <iostream>

int main(int argc, char** argv) {
    std::cout << DUSK_LOGO << std::endl << std::endl;
    filesystem::check_filesystem();
    args::process_args(argc, argv);
    return 0;
}
