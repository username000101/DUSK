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
}
