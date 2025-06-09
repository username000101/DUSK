#include "Args.hpp"

#include <cstdint>
#include <filesystem>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>
#include <td/telegram/td_api.h>

#include "Callbacks.hpp"

namespace filesystem {
    void check_filesystem();
}

int args::process_args(int argc, char **argv) {
    CLI::App DUSK("Telegram userbot");

    bool remove_user_flag = false, show_version = false, reinit_config_flag = false, update_config_flag = false, show_modules;
    std::int64_t user;
    std::filesystem::path custom_config_file;

    auto user_param = DUSK.add_option("-u,--user", user,"Specify the user(chat_id)");
    user_param->type_name("INT64");

    auto remove_user = DUSK.add_flag("-r,--remove", remove_user_flag, "Remove the specified user");
    remove_user->type_name("BOOL");
    remove_user->needs(user_param);

    auto custom_cfg = DUSK.add_option("-f,--cfg,--config", custom_config_file, "Run DUSK with custom configuration file");
    custom_cfg->type_name("FILE");

    DUSK.add_flag("-v,--version", show_version, "Show DUSK version");

    DUSK.add_flag("-w,--reinit", reinit_config_flag, "Restore the default config");
    DUSK.add_flag("-a,--update", update_config_flag, "Update the config");

    auto modules = DUSK.add_flag("-m,--modules", show_modules, "Show USER modules");
    modules->needs(user_param);

    DUSK.callback([&] () {
        if (show_version) /* --version */
            callbacks::show_version();

        if (remove_user_flag) /* --user USER --remove */
            callbacks::remove_user(user);

        if (reinit_config_flag) /* --reinit */
            callbacks::reinit_config();

        if (show_modules)
            callbacks::modules_list(user);

        if (update_config_flag)
            callbacks::update_config();

        if (!custom_config_file.empty()) /* --config=FILE */ { /* UNDECLARED YET */ }

        spdlog::info("Not found any tasks(?)... Exit!");
    });
    CLI11_PARSE(DUSK, argc, argv);
    return 0;
}
