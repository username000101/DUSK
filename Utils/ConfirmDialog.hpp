#pragma once

#include <string>

#include <FTXUI/component/component.hpp>
#include <FTXUI/component/screen_interactive.hpp>


inline bool confirm(const std::string& promt) {
    bool confirmed;
    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    auto dialog = ftxui::Container::Horizontal({
        ftxui::Button("Yes", [&] { confirmed = true; screen.ExitLoopClosure()(); }),
        ftxui::Button("No", [&] { confirmed = false; screen.ExitLoopClosure()(); }),
        });

    auto renderer = Renderer(dialog, [&] {
        return window(ftxui::text(promt) | ftxui::center,
            hbox(dialog->Render()) | ftxui::center);
    });

    screen.Loop(renderer);

    if (confirmed)
        return true;
    else
        return false;
}