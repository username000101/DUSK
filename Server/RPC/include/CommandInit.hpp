#pragma once

#include <string>

#include <spdlog/spdlog.h>

#include "PrivilegedRequest.hpp"
#include "Macros.hpp"

inline void dusk_rpc_server_command_init(const std::string& access_token) {
    if (!check_access(access_token, FUNCSIG))
        return;

    spdlog::info("The init is incomplete now");
}