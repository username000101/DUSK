#pragma once

#include <cstdint>
#include <vector>

#include <rpc/server.h>
#include <subprocess.hpp>

#include "Configuration.hpp"

namespace globals {
    inline std::int64_t current_user;
    inline std::shared_ptr<config::Configuration> configuration;
    inline std::shared_ptr<rpc::server> rpc_server;
    inline std::vector<subprocess::Popen> detached_processes;
}
