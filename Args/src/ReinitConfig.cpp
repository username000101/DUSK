#include "Callbacks.hpp"

#include <filesystem>

#include <spdlog/spdlog.h>

namespace filesystem {
    void check_filesystem();
}

void args::callbacks::reinit_config() {
    if (std::filesystem::exists(DUSK_CONFIG) && !std::filesystem::is_directory(DUSK_CONFIG)) {
        spdlog::warn("Deleting current {} file",
                     DUSK_CONFIG);
        std::filesystem::remove(DUSK_CONFIG);
    }
    filesystem::check_filesystem();
    std::exit(0);
}
