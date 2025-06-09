#pragma once

#include <string>

#include <nlohmann/json.hpp>

namespace filesystem {
    void check_filesystem();
    inline std::string get_dusk_home() { return DUSK_HOME; }
    nlohmann::json default_config();
}
