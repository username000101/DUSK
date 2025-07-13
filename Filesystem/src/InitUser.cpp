#include "FS.hpp"

#include <fstream>

#include <spdlog/spdlog.h>

#include "Globals.hpp"

void filesystem::init_user() {
    if (globals::current_user != 0) {
        auto user_dir = std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user);
        std::filesystem::create_directories(user_dir);
        std::filesystem::create_directory(user_dir / "modules");
        std::filesystem::create_directory(user_dir / "database");
        std::filesystem::create_directory(user_dir / "files");
        if (!std::filesystem::exists(std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user) / "config.json")) {
            {
                std::ofstream config_file(std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user) / "config.json");
                if (!config_file.is_open()) {
                    spdlog::error("{}: Failed to create config file for user {}",
                        FUNCSIG, globals::current_user);
                }

                config_file << fmt::format("{{\n\t\"id\": {}\n}}",
                    globals::current_user) << std::endl;
            }
            spdlog::info("{}: Config for user {} has been created",
                FUNCSIG, globals::current_user);
        }
    }
}