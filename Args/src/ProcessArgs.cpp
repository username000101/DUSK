#include "Args.hpp"

#include <clocale>
#include <cstdint>
#include <filesystem>
#include <stdexcept>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include "Auth.hpp"
#include "Callbacks.hpp"
#include "Globals.hpp"
#include "FS.hpp"
#include "RPCServer.hpp"
#include "InitAccessTokens.hpp"
#include "Start.hpp"
#include "TerminateHandler.hpp"
#include "Updates.hpp"

namespace filesystem {
    void check_filesystem();
}

int args::process_args(int argc, char **argv) {
    globals::configuration = std::make_shared<config::Configuration>(config::Configuration::parse_file(DUSK_CONFIG));

    CLI::App DUSK("Telegram userbot");

    bool remove_user_flag = false, show_version = false, reinit_config_flag = false, update_config_flag = false, show_modules = false;
    std::int64_t user = 0;
    std::filesystem::path custom_config_file = "", install_file_path = "";

    auto user_param = DUSK.add_option("-u,--user", user,"Specify the user(chat_id)");
    user_param->type_name("INT64");

    auto remove_user = DUSK.add_flag("-r,--remove", remove_user_flag, "Remove the specified user");
    remove_user->needs(user_param);

    auto custom_cfg = DUSK.add_option("-f,--cfg,--config", custom_config_file, "Run DUSK with custom configuration file");
    custom_cfg->type_name("FILE");
    custom_cfg->needs(user_param);

    DUSK.add_flag("-v,--version", show_version, "Show DUSK version");

    DUSK.add_flag("-w,--reinit", reinit_config_flag, "Restore the default config");
    DUSK.add_flag("-a,--update", update_config_flag, "Update the config");

    DUSK.callback([&] () {
        if (globals::configuration->__has_main_module)
            init_access_tokens(globals::configuration->modules);

        if (user == 0 && (!show_version && !reinit_config_flag && !update_config_flag))
            shutdown(EXIT_FAILURE, "The --user/-u parameter is required");

        globals::current_user = user;
        filesystem::check_filesystem();

        if (show_version) /* --version */
            callbacks::show_version();

        if (remove_user_flag) /* --user USER --remove */
            callbacks::remove_user(user);

        if (reinit_config_flag) /* --reinit */
            callbacks::reinit_config();

        if (update_config_flag) /* --update */
            callbacks::update_config();

        if (!custom_config_file.empty()) /* --config=FILE */ { /* UNDECLARED YET */ }

        init_access_tokens(globals::configuration->modules);
        filesystem::init_user();

        std::string platform;
#if defined(OS_WINDOWS)
        platform = "Windows";
#elif defined(OS_LINUX)
        platform = "Linux";
#elif defined(OS_ANDROID)
        platform = "Android";
#else
        platform = "Unknown";
#endif

        auto loc = std::setlocale(LC_ALL, "");
        if (!loc)
            shutdown(EXIT_FAILURE, "std::setlocale() failed");
#if defined(DUSK_TDLIB_USE_TEST_DC)
        auth::setTdlibParameters(std::make_shared<td::ClientManager>(), DUSK_TDLIB_USE_TEST_DC, true, true, true, true, API_ID, API_HASH, loc, platform, platform, DUSK_VERSION);
#else
        auth::setTdlibParameters(std::make_shared<td::ClientManager>(), false, true, true, true, true, API_ID, API_HASH, loc, platform, platform, DUSK_VERSION);
#endif

        dusk::start();
    });
    CLI11_PARSE(DUSK, argc, argv);
    shutdown(0, "Not found any tasks(?)... Exit!");
    return 0;
}
