// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include <string>

class capabilities;
class options;

enum class color {
    unknown = 0,

    // Fixed colors
    red = 1,
    yellow = 3,
    blue = 4,
    cyan = 6,
    white = 7,
    snake = color::red,

    // Components which change color
    text = 2,
    time = 5,
    crouton_1 = 9,
    crouton_2 = 13,
    wall = 15,

    // Monochrome attributes
    mono_normal = 1,
    mono_bright = 2,
    mono_blinking = 3,
};

namespace palette {
    static constexpr auto red = "72;0;0";
    static constexpr auto orange = "100;72;0";
    static constexpr auto green = "59;72;59";
    static constexpr auto blue = "0;0;87";
    static constexpr auto cyan = "0;72;59";
    static constexpr auto purple = "72;0;87";
    static constexpr auto gray = "72;72;59";

    static constexpr auto bright_red = "100;0;0";
    static constexpr auto bright_yellow = "100;100;0";
    static constexpr auto bright_green = "0;100;0";
    static constexpr auto bright_cyan = "0;59;87";
    static constexpr auto bright_purple = "100;0;87";
    static constexpr auto white = "100;100;87";

    static constexpr auto white_blue = "81;81;90";
    static constexpr auto red_purple = "84;25;41";
    static constexpr auto red_orange = "84;38;25";
    static constexpr auto green_blue = "58;65;74";
    static constexpr auto purple_orange = "84;38;73";
    static constexpr auto green_red = "38;81;25";
    static constexpr auto blue_yellow = "43;43;73";
    static constexpr auto cyan_red = "38;58;73";
}  // namespace palette

class coloring {
public:
    coloring(const capabilities& caps, const options& options);
    ~coloring();

private:
    bool _using_colors;
    std::string _color_assignment;
    std::string _color_table;
};
