#include "Args.hpp"
#include "FS.hpp"

#include "Macros.hpp"

#include <iostream>

#include <spdlog/spdlog.h>

#include "Auth.hpp"

int main(int argc, char** argv) {
    /*std::cout << DUSK_LOGO << std::endl << std::endl;
    filesystem::check_filesystem();
    args::process_args(argc, argv);*/

    spdlog::set_level(spdlog::level::trace);
    auth::setTdlibParameters(std::make_shared<td::ClientManager>(), false, DUSK_HOME, DUSK_HOME, true, true, true, true, 21768531, "b8a2f88bbb0416c8459182d99515170b", "ru_RU", "Linux", "Linux", "1.0.0");

    return 0;
}
