#include "Loop.hpp"

#include <filesystem>
#include <string>
#include <memory>

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Globals.hpp"
#include "Updates.hpp"

void dusk::loop() {
    static auto logger = std::make_shared<spdlog::logger>("DUSK::loop", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});

    logger->info("DUSK is launched!");

    auto get_me_res = update::send_request(td::td_api::make_object<td::td_api::getMe>());
    if (get_me_res.object) {
        auto get_me = td::move_tl_object_as<td::td_api::user>(get_me_res.object);
        std::string account_details = fmt::format("{{\n\tfirst_name: {}\n\tusername(latest): {}\n\tDUSK_ACCOUNT: {}\n\tModules installed: {}\n}}",
                                                   get_me->first_name_, *get_me->usernames_->active_usernames_.begin(),
                                                   (std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user)).string(),
                                                   []() -> int {
                                                       int result = 0;
                                                       for (auto f : std::filesystem::directory_iterator(std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(globals::current_user) / "modules"))
                                                           ++result;
                                                       return result;
                                                   }());
        logger->info("Account details: {}",
                 account_details);
    }
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
