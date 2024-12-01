// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

class capabilities;
class options;
class screen;
class soft_font;

class engine {
public:
    static constexpr int width = 38;
    static constexpr int height = 22;

    engine(const capabilities& caps, const options& options, soft_font& font);
    bool run();

private:
    void _display_time_out(screen& screen);
    void _display_game_over(screen& screen);

    const capabilities& _caps;
    const options& _options;
    soft_font& _font;
};
