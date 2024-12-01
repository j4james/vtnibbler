// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include <array>
#include <string>

class screen;

class level {
public:
    level(screen& screen, const int wave);
    int points_per_crouton() const;
    void init_map();
    void init_croutons();
    void update(const int elapsed_frames);
    bool is_path(const int snake_y, const int snake_x) const;
    bool eat_crouton(const int snake_y, const int snake_x);
    bool complete() const;

private:
    void _build_map();
    void _build_croutons();
    void _build_palette();
    int _wall_shape(const int y, const int x) const;
    bool _is_path(const int y, const int x) const;
    bool& _is_path(const int y, const int x);

    screen& _screen;
    int _wave = 0;
    std::string _palette_macro_1;
    std::string _palette_macro_2;
    std::array<bool, 21 * 21> _path = {};
    std::array<bool, 17 * 17> _croutons = {};
    int _croutons_remaining = 0;
    int _frame = 0;
};
