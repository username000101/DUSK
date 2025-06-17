#include "Callbacks.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Macros.hpp"

void args::callbacks::modules_list(const std::int64_t& user_chat_id) {
    for (auto& user_dir : std::filesystem::directory_iterator(DUSK_ACCOUNTS)) {
        if (std::stoll(user_dir.path().filename().string()) != user_chat_id)
            continue;

        nlohmann::json user_config;
        {
            std::ifstream user_config_file(user_dir.path() / "config.json");
            if (!user_config_file.is_open()) {
                spdlog::warn("{}: Failed to open user \"{}\" config file",
                             FUNCSIG, user_dir.path().filename().string());
                std::exit(1);
            }

            user_config = nlohmann::json::parse(user_config_file);
        }

        for (auto& module : user_config["modules"].items()) {
            if (!module.value().is_object()) {
                spdlog::warn("{}: Incorrect module object; skipping...",
                             FUNCSIG);
                continue;
            }

            std::cout << module.value().dump(4) << std::endl << std::endl;
        }
        std::exit(0);
    }

    spdlog::warn("{}: Not found modules from user \"{}\"",
                 FUNCSIG, user_chat_id);
    std::exit(0);
}
