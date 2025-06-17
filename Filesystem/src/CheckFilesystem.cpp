#include "FS.hpp"

#include <filesystem>
#include <fstream>

#include <spdlog/spdlog.h>

#include "Globals.hpp"
#include "Macros.hpp"

void write_default_config() {
    {
        std::ofstream config_file(DUSK_CONFIG);
        if (!config_file.is_open()) {
            spdlog::error("{}: Failed to open DUSK config file({})",
                          FUNCSIG, DUSK_CONFIG);
            return;
        }
        config_file << filesystem::default_config().dump(4);
    }
    spdlog::info("{}: DUSK default config will be wrote",
                     FUNCSIG);
}

void filesystem::check_filesystem() {
    if (!std::filesystem::exists(DUSK_HOME) || !std::filesystem::is_directory(DUSK_HOME))
        std::filesystem::create_directories(DUSK_HOME);
    if (!std::filesystem::exists(DUSK_CONFIG) || std::filesystem::is_directory(DUSK_CONFIG))
        write_default_config();
    if (!std::filesystem::exists(DUSK_ACCOUNTS) || !std::filesystem::is_directory(DUSK_ACCOUNTS))
        std::filesystem::create_directory(DUSK_ACCOUNTS);

    if (globals::current_user != 0) {
	auto user_dir = std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user);
        std::filesystem::create_directories(user_dir);
	std::filesystem::create_directory(user_dir / "modules");
    }
}
