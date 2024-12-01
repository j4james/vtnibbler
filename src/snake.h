// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include "coloring.h"

#include <array>
#include <string_view>
#include <tuple>
#include <vector>

class level;
class screen;

class snake {
public:
    snake(screen& screen, const level& level);
    void init();
    bool turn(const int dy, const int dx);
    void move();
    void grow();
    bool erase();
    std::tuple<int, int> position() const;
    bool just_eaten() const;
    bool is_dead() const;

private:
    struct segment {
        int y;
        int x;
    };

    bool _can_move(const int dy, const int dx) const;
    void _render_head();
    void _render_tail();
    void _render(const int y, const int x, const std::string_view s, const color color = color::red);
    void _track_occupation(const int y, const int x, const bool occupied);
    bool _is_occupied(const int y, const int x) const;

    screen& _screen;
    const level& _level;
    std::vector<segment> _body;
    std::array<bool, 17 * 17> _occupied = {};
    int _dy = 0;
    int _dx = 0;
    int _paused = 0;
    int _growing = 0;
    bool _just_eaten = false;
    bool _dead = false;
};
