#pragma once

#include <unordered_map>
#include <string>

extern const std::unordered_map<std::string, std::pair<std::string, std::string>>& get_access_tokens();

inline bool check_access(const std::string& access_token, const std::string& request) {
    static auto logger = std::make_shared<spdlog::logger>("RPC Access Manager", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
    spdlog::initialize_logger(logger);

    auto& access_tokens = get_access_tokens();
    if (access_tokens.contains(access_token)) {
        auto& pair = access_tokens.at(access_token);
        logger->info("Call '{}' from '{}'",
            request, pair.first);
        logger->debug("Additional debug info: '{}' is a '{}'",
            pair.first, pair.second);
        return true;
    } else {
        logger->warn("Attempt to call '{}' from 'unknown' because token '{}' is invalid",
            request, access_token);
        return false;
    }
}