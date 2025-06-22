#pragma once

#include <cstdint>

#include "Configuration.hpp"

namespace globals {
    inline std::int64_t current_user;
    inline std::shared_ptr<config::Configuration> configuration;
}
