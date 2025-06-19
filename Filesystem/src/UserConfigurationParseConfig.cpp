#include "Configuration.hpp"

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
        spdlog::info("{}: Overriding prefix: \".\" ==> \"{}\"",
                     FUNCSIG, prefix);
        this->prefix_ = prefix;
    }

    if (!config.contains("id")) {
        spdlog::warn("{}: Not found id in config attempt to get from path...{}",
                     FUNCSIG, this->account_directory().filename().string());
        this->id_ = std::stoll(this->account_directory().filename().string());
    } else
        this->id_ = config["id"].template get<int64_t>();
    for (auto& kv_pair : config["modules"].items()) {
        auto module = kv_pair.value();
        Module module_obj;

        if (!module.contains("name")) {
            spdlog::warn("{}: Incorrect module config(!module.contains(\"name\")); module dump:\n{}",
                         FUNCSIG, module.dump(4));
            continue;
        }
        module_obj.name = module["name"].template get<std::string>();

        if (!module.contains("id")) {
            spdlog::warn("{}: Incorrect module config(!module.contains(\"id\")); module dump:\n{}",
                         FUNCSIG, module.dump(4));
            continue;
        }
        module_obj.id = module["id"].template get<std::string>();

        if (!module.contains("author")) {
            spdlog::warn("{}: Incomplete module config(!module.contains(\"author\")); THIS IS NOT AN ERROR default value is \"UNKNOWN\"",
                         FUNCSIG);
            module_obj.author = "UNKNOWN";
        } else
            module_obj.author = module["author"].template get<std::string>();

        if (!module.contains("version")) {
            spdlog::warn("{}: Incomplete module config(!module.contains(\"version\")); THIS IS NOT AN ERROR default value is \"0.0.0\"");
            module_obj.version = "0.0.0";
        } else
            module_obj.version = module["version"].template get<std::string>();

        if (!module.contains("description")) {
            spdlog::warn("{}: Incomplete module config(!module.contains(\"description\")); THIS IS NOT AN ERROR default value is \"\"");
            module_obj.description = "";
        } else
            module_obj.description = module["description"].template get<std::string>();

        for (auto& kv_pair_epoints : module["entry_points"].items()) {
            module_obj.entry_points.emplace_back(kv_pair_epoints.key(), kv_pair_epoints.value().template get<std::string>());
        }
        spdlog::info("{}: Founded {} entry points",
                     FUNCSIG, module_obj.entry_points.size());
        this->modules_.emplace_back(std::move(module_obj));
    }
}
