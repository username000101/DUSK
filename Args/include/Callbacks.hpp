#pragma once

#include <cstdint>
#include <filesystem>

namespace args {
    namespace callbacks {
        void remove_user(const std::int64_t& user_chat_id);
        void show_version();
        void update_config();
        void reinit_config();
        void modules_list(const std::int64_t& user_chat_id);
        void install(const std::filesystem::path& file);
    }
}
