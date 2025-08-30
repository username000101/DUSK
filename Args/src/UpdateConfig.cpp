#include "Callbacks.hpp"

#include <fstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Macros.hpp"
#include "TerminateHandler.hpp"

void args::callbacks::update_config() {
    nlohmann::json config;
    { /* Reading section */
        std::ifstream config_read(DUSK_CONFIG);
        if (!config_read.is_open()) {
            spdlog::error("{}: Failed to open config file for reading",
                          FUNCSIG);
            std::exit(1);
        }
        config = nlohmann::json::parse(config_read);
    }

    config["version"] = DUSK_VERSION;

    { /* Writing section */
        std::ofstream config_write(DUSK_CONFIG);
        if (!config_write.is_open()) {
            spdlog::error("{}: Failed to open config file for writing",
                          FUNCSIG);
            std::exit(1);
        }

        config_write << config.dump(4);
    }

    shutdown(0, "Update config complete");
}