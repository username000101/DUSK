#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Args.hpp"
#include "Macros.hpp"
#include "TerminateHandler.hpp"

int main(int argc, char** argv) {
    auto defcon = std::make_shared<spdlog::logger>("Default", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});
    spdlog::set_default_logger(defcon);
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);
#endif
    std::signal(SIGABRT, signal_handler_f);
    std::signal(SIGTERM, signal_handler_f);
    std::signal(SIGILL, signal_handler_f);
    std::signal(SIGINT, signal_handler_f);
    std::set_terminate(terminate_handler_f);
    std::cout << DUSK_LOGO << std::endl << std::endl;
    args::process_args(argc, argv);
    return 0;
}
