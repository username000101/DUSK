#include "Configuration.hpp"

#include <cstdint>
#include <format>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Globals.hpp"
#include "Macros.hpp"
#include "TerminateHandler.hpp"

config::Configuration config::Configuration::parse_file(const std::filesystem::path &file) {
    static auto logger = std::make_shared<spdlog::logger>("Configuration", spdlog::sinks_init_list{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() });
    spdlog::initialize_logger(logger);

    if (!std::filesystem::exists(file))
        shutdown(EXIT_FAILURE, "Failed to create class Configuration instance: file(" + file.string() + ") does not exist");

    nlohmann::json dusk_config;

    {
        std::ifstream read_dusk_config(file);
        if (!read_dusk_config.is_open())
            shutdown(EXIT_FAILURE, "Failed to create class Configuration instance: failed to open file(" + file.string() + ")");
        dusk_config = nlohmann::json::parse(read_dusk_config);
    }

    Configuration result;

    if (!dusk_config.contains("version"))
        shutdown(EXIT_FAILURE, "Failed to create class Configuration instance: !dusk_config.contains(\"version\")");
    result.version = dusk_config["version"].template get<std::string>();

    std::map<std::string, std::string> local_modids_map;
    for (auto& early_load_module : dusk_config["modules"].items()) {
        auto& value = early_load_module.value();

        BaseModuleInfo el_module;
        el_module.admin = true;

        if (!value.contains("file") || (value.contains("file") && !std::filesystem::exists(value.at("file").template get<std::string>())))
            shutdown(EXIT_FAILURE, std::format("Failed to load dusk-level module: not found the 'file' field or file({}) does not exist", (value.contains("file") ? value.at("file").template get<std::string>() : "FIELD_NOT_FOUND")));
        el_module.file = value.at("file").template get<std::string>();

        if (!value.contains("id") || (value.contains("id") && local_modids_map.contains(value.at("id").template get<std::string>())))
            shutdown(EXIT_FAILURE, "Failed to load dusk-level module: not found the 'id' field or this id already exists");
        el_module.mod_id = value.at("id").template get<std::string>();
        local_modids_map[el_module.mod_id] = el_module.file.string();

        if (value.contains("prefix"))
            el_module.prefix = value.at("prefix").template get<std::string>();

        if (!value.contains("port"))
            shutdown(EXIT_FAILURE, "Failed to load dusk-level module: not found the 'port' field");
        else
            el_module.rpc_port = value.at("port").template get<std::uint32_t>();

        if (!value.contains("set_access_token_function"))
            shutdown(EXIT_FAILURE, "Failed to load dusk-level module: not found the 'set_access_token_function' field");
        else
            el_module.set_access_token_function = value.at("set_access_token_function").template get<std::string>();

        if (!value.contains("main")) {
            logger->debug("At file '{}': not found the 'main' field(default value is 'false')",
                el_module.file.string());
            el_module.main = false;
        } else
            el_module.main = value.at("main").template get<bool>();
        if (el_module.main)
            result.__has_main_module = true;
        else
            result.__has_main_module = false;

        result.modules.push_back(el_module);
    }

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
        if (user_obj() == globals::current_user)
            result.current_user = user_obj;
        result.users.push_back(user_obj);
    }

    return result;
}
