#include "Callbacks.hpp"

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Macros.hpp"

void args::callbacks::show_version() {
    std::ifstream config_file(DUSK_CONFIG);
    std::string config_file_version;

    if (!config_file.is_open()) {
        spdlog::error("Failed to open config file {}",
                        DUSK_CONFIG);
    } else {
        nlohmann::json config = nlohmann::json::parse(config_file);
        config_file.close();
        config_file_version = config["version"];
    }

    spdlog::info(":\nVersion(MACROS): {}\nVersion(CONFIG({})): {}{}",
                 DUSK_VERSION, DUSK_CONFIG, config_file_version,
                 (config_file_version != DUSK_VERSION ? "\n\nIf the MACROS version and CONFIG version are different, it is likely that DUSK has been updated but the configuration file has not. To fix this, you can overwrite the configuration file (all custom settings will be deleted)(DUSK --reinit), or update (all custom settings will be retained)(DUSK --update)." : ""));
    std::exit(0);
}
