#pragma once

#include <cstdint>

#include <rpc/server.h>

#include "Configuration.hpp"

namespace globals {
    inline std::int64_t current_user;
    inline std::shared_ptr<config::Configuration> configuration;
    inline std::shared_ptr<rpc::server> rpc_server;
}
