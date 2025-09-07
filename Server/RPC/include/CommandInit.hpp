#pragma once

#include <string>

#include <spdlog/spdlog.h>

#include "Auth.hpp"
#include "FS.hpp"
#include "Globals.hpp"
#include "Macros.hpp"
#include "PrivilegedRequest.hpp"
#include "Start.hpp"

inline void dusk_rpc_server_command_init(const std::string& access_token) {
    if (!check_access(access_token, FUNCSIG))
        return;

    if (globals::current_user == 0) {
        spdlog::error("{}: To use dusk.init command you need to specify the current_user variable(use dusk.set_user)",
            FUNCSIG);
        return;
    }

    filesystem::check_filesystem();
    filesystem::init_user();


    std::string platform;
#if defined(OS_WINDOWS)
    platform = "Windows";
#elif defined(OS_LINUX)
    platform = "Linux";
#elif defined(OS_ANDROID)
    platform = "Android";
#else
    platform = "Unknown";
#endif

    std::locale loc("");
    #if defined(DUSK_TDLIB_USE_TEST_DC)
    auth::setTdlibParameters(std::make_shared<td::ClientManager>(), DUSK_TDLIB_USE_TEST_DC, true, true, true, true, API_ID, API_HASH, loc.name(), platform, platform, DUSK_VERSION);
#else
    auth::setTdlibParameters(std::make_shared<td::ClientManager>(), false, true, true, true, true, API_ID, API_HASH, loc.name(), platform, platform, DUSK_VERSION);
#endif

    dusk::start();
}