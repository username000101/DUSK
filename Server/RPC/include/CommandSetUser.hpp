#pragma once

#include <cstdint>
#include <string>

#include <spdlog/spdlog.h>

#include "Globals.hpp"
#include "Macros.hpp"
#include "PrivilegedRequest.hpp"

inline void dusk_rpc_server_command_set_user(const std::string& access_token, std::int64_t user) {
    if (!check_access(access_token, FUNCSIG))
        return;

    spdlog::warn("{}: Set the current_user variable to {}",
        FUNCSIG, user);
    globals::current_user = user;
}