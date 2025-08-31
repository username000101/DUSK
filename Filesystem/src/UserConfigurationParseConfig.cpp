#include "Configuration.hpp"

#include <cstdint>
#include <fstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "Macros.hpp"

void config::UserConfiguration::inl_parse_config() {
    nlohmann::json config;
    {
        std::ifstream read_config(this->configuration_file_);
        if (!read_config.is_open()) {
            throw std::runtime_error("Failed to create class UserConfiguration instance: UserConfiguration::inl_parse_config(): failed to open configuration_file_(" + this->configuration_file_.string() + ")");
        }
        config = nlohmann::json::parse(read_config);
    }

    if (!config.contains("prefix"))
        this->prefix_ = ".";
    else {
        auto prefix = config["prefix"].template get<std::string>();
        spdlog::info(R"({}: Overriding prefix: "." ==> "{}")",
                     FUNCSIG, prefix);
        this->prefix_ = prefix;
    }

    if (!config.contains("id")) {
        spdlog::warn("{}: Not found id in config attempt to get from path...{}",
                     FUNCSIG, this->account_directory().filename().string());
        this->id_ = std::stoll(this->account_directory().filename().string());
    } else
        this->id_ = config["id"].template get<int64_t>();

    if (config.contains("blocked_requests")) {
        for (auto& req_id : config.at("blocked_requests").items()) {
            this->blocked_requests_.push_back(req_id.value().template get<std::int64_t>());
        }
        spdlog::info("{}: Added {} blocked requests",
            FUNCSIG, this->blocked_requests_.size());
    }

    for (auto& kv_pair : config["modules"].items()) {
        BaseModuleInfo base_module_info;
        const auto& base_module_info_json = kv_pair.value();

        if (!base_module_info_json.contains("file")) {
            spdlog::error("{}: Not found required field 'file' in base module info",
                FUNCSIG);
            continue;
        }
        if (const auto file_ = base_module_info_json.at("file"); !std::filesystem::exists(file_.template get<std::string>())) {
            spdlog::error("{}: File '{}' does not exist",
                FUNCSIG, file_.template get<std::string>());
            continue;
        }
        base_module_info.file = base_module_info_json.at("file").template get<std::string>();

        if (!base_module_info_json.contains("port")) {
            spdlog::error("{}: Not found required field 'port' in base module info",
                FUNCSIG);
            continue;
        }
        if (const auto port_ = base_module_info_json.at("port").template get<std::uint16_t>(); port_ > 65535) {
            spdlog::error("{}: Invalid port number: {}",
                FUNCSIG, port_);
            continue;
        }
        base_module_info.rpc_port = base_module_info_json.at("port").template get<std::uint16_t>();

        if (!base_module_info_json.contains("function")) {
            spdlog::error("{}: Not found required field 'function' in base module info",
                FUNCSIG);
            continue;
        }
        base_module_info.get_config_rpc_function = base_module_info_json.at("function").template get<std::string>();

        if (!base_module_info_json.contains("prefix")) {
            spdlog::error("{}: Not found required field 'prefix' in base module info",
                FUNCSIG);
            continue;
        }
        base_module_info.prefix = base_module_info_json.at("prefix").template get<std::string>();

        /* 'admin' and 'main' variables only for DUSK-level modules*/
        base_module_info.admin = false;
        base_module_info.main = false;

        spdlog::debug("{}: Loaded module_base:\nFile: {}\nPort: {}\nFunction: {}",
            FUNCSIG, base_module_info.file.string(), base_module_info.rpc_port, base_module_info.get_config_rpc_function);
        this->modules_base_.push_back(std::move(base_module_info));
    }

    spdlog::info("{}: Loaded {} modules(modules_base)",
        FUNCSIG, this->modules_base_.size());
}
