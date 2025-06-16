#include "Callbacks.hpp"

#include <spdlog/spdlog.h>

#include "InstallModpack.hpp"

void args::callbacks::install(const std::filesystem::path& file) {
    if (!std::filesystem::exists(file)) {
        spdlog::error("File doesn't exist");
        return;
    }

    modpack::install_modpack<std::map>(file);
}
