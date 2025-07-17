#pragma once

#include <algorithm>
#include <optional>
#include <string>

#include <format>

#include "Globals.hpp"

inline std::string dusk_rpc_server_command_module(const std::string& id_n) {
    auto modules = globals::configuration->current_user.get_modules();
    std::optional<config::Module> result = std::nullopt;

    std::ranges::for_each(modules, [&](const auto& module) {
        if ((module.id == id_n || module.name == id_n) && !result.has_value())
            result = module;
    });

    if (result.has_value()) {
        auto& module = result.value();
        return std::format(R"({{"id":"{}","name":"{}","author":"{}","description":"{}","version":"{}"}})",
            module.id, module.name, module.author, module.description, module.version);
    }
    return "";


}