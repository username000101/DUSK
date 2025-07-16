#include "Start.hpp"

#include <filesystem>
#include <string>
#include <iostream>
#include <memory>

#include <rpc/server.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <td/telegram/td_api.h>

#include "Auth.hpp"
#include "Configuration.hpp"
#include "Events.hpp"
#include "Globals.hpp"
#include "RPCServer.hpp"
#include "TerminateHandler.hpp"
#include "Updates.hpp"

void dusk::start() {
    static auto logger = std::make_shared<spdlog::logger>("DUSK::start", spdlog::sinks_init_list{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()});

    spdlog::debug("Trying to get current authorization state...");
    while (true) {
        auto current_authorization_state = update::send_request(td::td_api::make_object<td::td_api::getAuthorizationState>());
        if (!current_authorization_state.object) {
            logger->warn("Failed to get current authorization state: response::object is null");
            continue;
        }

        if (current_authorization_state.object->get_id() != td::td_api::authorizationStateReady::ID) {
            auth::authorize();
            break;
        }
        spdlog::debug("Current authorization state is authorizationStateReady");
        break;
    }

    logger->info("DUSK is launched!");
    auto get_me_res = update::send_request(td::td_api::make_object<td::td_api::getMe>());
    if (get_me_res.object) {
        auto get_me = td::move_tl_object_as<td::td_api::user>(get_me_res.object);
        std::string account_details = fmt::format("{{\n\tfirst_name: {}\n\tusername(latest): {}\n\tDUSK_ACCOUNT: {}\n\tModules installed: {}\n}}",
                                                   get_me->first_name_, (!get_me->usernames_ ? std::to_string(get_me->id_) : *get_me->usernames_->active_usernames_.begin()),
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

    globals::configuration = std::make_shared<config::Configuration>(config::Configuration::parse_file(DUSK_CONFIG));
    if (globals::configuration->users.empty()) {
        logger->error("No users found! Please update DUSK configuration file");
        shutdown(EXIT_FAILURE);
    }
    logger->info("Configuration was successfully loaded!");

    logger->info("Raising up modules...");
    globals::configuration->current_user.load_modules();

    server::rpc::up_rpc_server();
}
