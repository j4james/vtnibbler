// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "capabilities.h"
#include "coloring.h"
#include "engine.h"
#include "font.h"
#include "options.h"
#include "os.h"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

static bool check_compatibility(const capabilities& caps, const options& options)
{
    if (!caps.has_soft_fonts && !options.yolo) {
        std::cout << "VT Nibbler requires a VT320-compatible terminal or better.\n";
        std::cout << "Try 'vtnibbler --yolo' to bypass the compatibility checks.\n";
        return false;
    }
    if (caps.height < engine::height) {
        std::cout << "VT Nibbler requires a minimum screen height of " << engine::height << ".\n";
        return false;
    }
    if (caps.width < engine::width) {
        std::cout << "VT Nibbler requires a minimum screen width of " << engine::width << ".\n";
        return false;
    }
    return true;
}

static auto title_banner(const capabilities& caps)
{
    constexpr auto title = "VT NIBBLER";
    const auto y = caps.height / 2;
    const auto x = caps.width / 4 - 4;
    std::cout << "\033[" << y << ';' << x << "H\033#3" << title;
    std::cout << "\033[" << (y + 1) << ';' << x << "H\033#4" << title;
    std::cout.flush();

    return [=]() {
        std::this_thread::sleep_for(3s);
        // MLTerm doesn't reset double-width lines correctly, so we need to
        // manually reset the title banner line before starting the game.
        std::cout << "\033[" << y << "H\033[2K\033#5";
        std::cout << "\033[" << (y + 1) << "H\033[2K\033#5";
        std::cout.flush();
    };
}

int main(const int argc, const char* argv[])
{
    os os;

    options options(argc, argv);
    if (options.exit)
        return 1;

    capabilities caps;
    if (!check_compatibility(caps, options))
        return 1;

    // Set the window title.
    std::cout << "\033]21;VT Nibbler\033\\";
    // Set default attributes.
    std::cout << "\033[m";
    // Clear the screen.
    std::cout << "\033[2J";
    // Save the modes and settings that we're going to change.
    const auto original_decawm = caps.query_mode(7);
    const auto original_decssdt = caps.query_setting("$~");
    // Hide the cursor.
    std::cout << "\033[?25l";
    // Disable line wrapping.
    std::cout << "\033[?7l";
    // Hide the status line.
    std::cout << "\033[0$~";
    // Display title banner
    const auto clear_banner = title_banner(caps);
    // Load the soft font.
    auto font = soft_font{caps};
    // Setup the color assignment and palette.
    const auto colors = coloring{caps, options};
    // Clear the title banner
    clear_banner();

    auto game_engine = engine{caps, options, font};
    while (game_engine.run()) {
    }

    // Clear the window title.
    std::cout << "\033]21;\033\\";
    // Set default attributes.
    std::cout << "\033[m";
    // Clear the screen.
    std::cout << "\033[H\033[J";
    // Reapply line wrapping if not originally reset.
    if (original_decawm != false)
        std::cout << "\033[?7h";
    // Restore the original status display type.
    if (!original_decssdt.empty())
        std::cout << "\033[" << original_decssdt;
    // Show the cursor.
    std::cout << "\033[?25h";

    return 0;
}
