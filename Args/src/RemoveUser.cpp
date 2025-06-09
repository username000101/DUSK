#include "Callbacks.hpp"

#include <filesystem>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <spdlog/spdlog.h>

#include "Convert.hpp"

void args::callbacks::remove_user(const std::int64_t& user_chat_id) {
    std::filesystem::path full_user_data_path = std::filesystem::path(DUSK_ACCOUNTS) / std::to_string(user_chat_id);
    if (std::filesystem::exists(full_user_data_path)) {
        auto screen = ftxui::ScreenInteractive::TerminalOutput();

        bool confirmed = false;

        auto dialog = ftxui::Container::Horizontal({
            ftxui::Button("Yes", [&] { confirmed = true; screen.ExitLoopClosure()(); }),
            ftxui::Button("No", [&] { confirmed = false; screen.ExitLoopClosure()(); }),
        });

        auto renderer = Renderer(dialog, [&] {
            return window(ftxui::text(R"(Are you sure want to delete user ")" + std::to_string(user_chat_id) + R"(")") | ftxui::center,
                    hbox(dialog->Render()) | ftxui::center);
        });

        screen.Loop(renderer);

        if (confirmed) {
            spdlog::warn(R"(Deleting user "{}")",
                         user_chat_id);
            std::filesystem::remove_all(full_user_data_path);
            std::exit(0);
        } else
            std::exit(1);
    } else {
        spdlog::warn(R"(User "{}" does not exist(search by user id "{}")",
                     user_chat_id, full_user_data_path.string());
    }

}
