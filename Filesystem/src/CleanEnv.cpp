#include "FS.hpp"

#include <spdlog/spdlog.h>

#include "Macros.hpp"

void filesystem::clean_env() {
    if (std::filesystem::exists(DUSK_TMP) && !std::filesystem::is_empty(DUSK_TMP)) {
        std::filesystem::remove_all(DUSK_TMP);
        spdlog::debug("{}: Deleting {}...",
                      FUNCSIG, DUSK_TMP);
    }
}
