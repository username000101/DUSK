#include "Configuration.hpp"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Globals.hpp"
#include "Macros.hpp"

config::Configuration config::Configuration::parse_file(const std::filesystem::path &file) {
    if (!std::filesystem::exists(file))
        throw std::runtime_error("Failed to create class Configuration instance: file(" + file.string() + ") does not exist");

    nlohmann::json dusk_config;

    {
        std::ifstream read_dusk_config(file);
        if (!read_dusk_config.is_open())
            throw std::runtime_error("Failed to create class Configuration instance: failed to open file(" + file.string() + ")");
        dusk_config = nlohmann::json::parse(read_dusk_config);
    }

    Configuration result;

    if (!dusk_config.contains("version"))
        throw std::runtime_error("Failed to create class Configuration instance: !dusk_config.contains(\"version\")");
    result.version = dusk_config["version"].template get<std::string>();

    for (auto& user : dusk_config["users"].items()) {
        auto user_config_file = user.value().template get<std::string>();
        if (!std::filesystem::exists(user_config_file)) {
            spdlog::warn("{}: The user config file({}) does not exist",
                         FUNCSIG, user_config_file);
            continue;
        }

        auto account_dir = std::filesystem::path(user_config_file).parent_path();
        auto modules_dir = account_dir / "modules";
        auto tdlib_files_dir = account_dir / "files";
        auto tdlib_database_dir = account_dir / "database";

        auto user_obj = UserConfiguration(account_dir, tdlib_files_dir, tdlib_database_dir, modules_dir);
        if (user_obj == globals::current_user)
            result.current_user = user_obj;
        result.users.push_back(user_obj);
    }

    return result;
}
