#pragma once

#include <exception>
#include <spdlog/spdlog.h>

#include "FS.hpp"
#include "Macros.hpp"

inline void terminate_handler_f() {
    auto current_exc = std::current_exception();
    try {
        std::rethrow_exception(current_exc);
    } catch (std::exception& error) {
        spdlog::error("{}: Exception: {}",
                      FUNCSIG, error.what());
    } catch (...) {
        spdlog::error("{}: Exception: UNKNOWN");
    }

    filesystem::clean_env();
    std::exit(3);
}