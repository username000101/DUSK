#include "Args.hpp"

#include <cstdint>
#include <filesystem>
#include <stdexcept>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include "Auth.hpp"
#include "Callbacks.hpp"
#include "Globals.hpp"
#include "FS.hpp"
#include "Start.hpp"
#include "Updates.hpp"

namespace filesystem {
    void check_filesystem();
}

int args::process_args(int argc, char **argv) {
    CLI::App DUSK("Telegram userbot");

    bool remove_user_flag = false, show_version = false, reinit_config_flag = false, update_config_flag = false, show_modules = false;
    std::int64_t user = 0;
    std::filesystem::path custom_config_file = "", install_file_path = "";
    std::thread update_thread;

    auto user_param = DUSK.add_option("-u,--user", user,"Specify the user(chat_id)");
    user_param->type_name("INT64");

    auto remove_user = DUSK.add_flag("-r,--remove", remove_user_flag, "Remove the specified user");
    remove_user->type_name("BOOL");
    remove_user->needs(user_param);

    auto custom_cfg = DUSK.add_option("-f,--cfg,--config", custom_config_file, "Run DUSK with custom configuration file");
    custom_cfg->type_name("FILE");
    custom_cfg->needs(user_param);

    DUSK.add_flag("-v,--version", show_version, "Show DUSK version");

    DUSK.add_flag("-w,--reinit", reinit_config_flag, "Restore the default config");
    DUSK.add_flag("-a,--update", update_config_flag, "Update the config");

    auto modules = DUSK.add_flag("-m,--modules", show_modules, "Show USER modules");
    modules->needs(user_param);

    auto install = DUSK.add_option("-i,--install", install_file_path, "Install the specified module package");
    install->type_name("FILE");
    install->needs(user_param);

    DUSK.callback([&] () {
        if (user == 0 && (!show_version && !reinit_config_flag && !update_config_flag))
            throw std::invalid_argument("The --user/-u parameter is required");

        globals::current_user = user;
        filesystem::check_filesystem();

        if (show_version) /* --version */
            callbacks::show_version();

        if (remove_user_flag) /* --user USER --remove */
            callbacks::remove_user(user);

        if (reinit_config_flag) /* --reinit */
            callbacks::reinit_config();

        if (show_modules) /* --modules */
            callbacks::modules_list(user);

        if (update_config_flag) /* --update */
            callbacks::update_config();

        if (!custom_config_file.empty()) /* --config=FILE */ { /* UNDECLARED YET */ }

        if (!install_file_path.empty()) /* --install=FILE */
            callbacks::install(install_file_path);

        update_thread = std::thread([] () { update::updates_broadcaster(); });
#if defined(DUSK_TDLIB_USE_TEST_DC)
        auth::setTdlibParameters(std::make_shared<td::ClientManager>(), DUSK_TDLIB_USE_TEST_DC, true, true, true, true, API_ID, API_HASH, "ru_RU", "Linux", "Linux", "1.0.0");
#else
        auth::setTdlibParameters(std::make_shared<td::ClientManager>(), false, true, true, true, true, API_ID, API_HASH, "ru_RU", "Linux", "Linux", "1.0.0");
#endif
        dusk::start();
        update_thread.join();
        spdlog::info("Not found any tasks(?)... Exit!");
    });
    CLI11_PARSE(DUSK, argc, argv);
    spdlog::shutdown();
    return 0;
}
