#pragma once

#include <algorithm>
#include <optional>
#include <string>

#include <format>

#include "Globals.hpp"
#include "PrivilegedRequest.hpp"

inline std::string dusk_rpc_server_command_module(const std::string& access_token, const std::string& id_n) {
    if (!check_access(access_token, FUNCSIG))
        return "access_denied";

    auto modules = globals::configuration->modules;
    std::optional<config::BaseModuleInfo> result = std::nullopt;

    std::ranges::for_each(modules, [&](const config::BaseModuleInfo& module) {
        if (module.mod_id == id_n && !result.has_value())
            result = module;
    });

    if (result.has_value()) {
        auto& module = result.value();
        return std::format(R"({{"id":"{}","file":"{}","main":"{}","rpc_port":"{}"}})",
            module.mod_id, module.file.string(), (module.main ? "true" : "false"), module.rpc_port);
    }
    return "not_found";


}