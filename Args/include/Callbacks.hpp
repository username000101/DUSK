#pragma once

#include <cstdint>
#include <filesystem>

namespace args {
    namespace callbacks {
        void remove_user(const std::int64_t& user_chat_id);
        void show_version();
        void update_config();
        void reinit_config();
    }
}
