#pragma once

#include "InstallModpack.hpp"

#include <unordered_map>

inline bool dusk_rpc_server_command_install_modpack(const std::string& modpack_path) {
    return modpack::install_modpack<std::unordered_map>(modpack_path);
}