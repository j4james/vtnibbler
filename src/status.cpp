// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "status.h"

#include "screen.h"

#include <algorithm>
#include <chrono>
#include <string>

using namespace std::chrono_literals;

namespace {

    std::string format_number(const int n)
    {
        auto str = std::to_string(n);
        for (auto i = 3; str.size() > i; i += 4)
            str.insert(str.size() - i, "~");
        return str;
    }

}  // namespace

int status::_high_score = 50000;

status::status(screen& screen)
    : _screen{screen}
{
}

status::~status()
{
    _high_score = std::max(_high_score, _score);
}

void status::init(const int wave)
{
    _frame = 0;
    _last_score_frame = 0;
    _screen.set_palette(color::time, palette::bright_yellow);
    _screen.write(1, 2, "PLAYER1", color::yellow);
    _render_score();
    _screen.write(1, 31, "LEFT", color::yellow);
    _render_lives();
    _screen.write(2, 2, "HISCORE", color::red);
    _render_high_score();
    _screen.write(2, 30, "TIME", color::time);
    _render_time();
    _screen.write(22, 16, "WAVE", color::white);
    _render_wave(wave);
    _screen.flush();
}

void status::update(const int elapsed_frames)
{
    const auto last_frame = _frame;
    _frame += elapsed_frames;
    const auto tick_rate = (_frame - _last_score_frame) >= 220 ? 5 : 30;
    if (_frame / tick_rate > last_frame / tick_rate) {
        _time -= 10;
        _render_time();
    }
}

void status::add_points(const int points)
{
    _last_score_frame = _frame;
    _score += points;
    _render_score();
}

void status::lose_life()
{
    _screen.set_palette(color::text, palette::purple);
    _screen.fill_color(1, 2, 2, 37, color::text);
    _screen.fill_color(22, 16, 22, 23, color::text);
    _screen.flush();
    _lives--;
}

void status::gain_life()
{
    _lives++;
}

void status::apply_bonus()
{
    for (auto i = 0; _time > 0; i++) {
        const auto bonus = std::min(_time, 40);
        _time -= bonus;
        _score += bonus;
        _render_score();
        _render_time();
        if (i % 2 == 0) _screen.play_sound(7);
        _screen.pause(32ms);
    }
}

void status::reset_time()
{
    _time = 990;
}

bool status::out_of_time() const
{
    return _time < 0;
}

bool status::game_over() const
{
    return _lives <= 0;
}

void status::_render_score()
{
    const auto score_string = format_number(_score);
    const auto x = 23 - score_string.length();
    _screen.write(1, x, score_string, color::white);
}

void status::_render_high_score()
{
    const auto score_string = format_number(_high_score);
    const auto x = 23 - score_string.length();
    _screen.write(2, x, score_string, color::cyan);
}

void status::_render_lives()
{
    auto lives_string = std::to_string(std::clamp(_lives - 1, 0, 99));
    lives_string.insert(0, 2 - lives_string.length(), ' ');
    _screen.write(1, 36, lives_string, color::white);
}

void status::_render_time()
{
    auto time_string = std::to_string(std::max(_time, 0));
    time_string.insert(0, 3 - time_string.length(), ' ');
    _screen.write(2, 35, time_string, color::white);
}

void status::_render_wave(const int wave)
{
    const auto wave_string = std::to_string(wave);
    const auto x = 24 - wave_string.length();
    _screen.write(22, x, wave_string, color::white);
}
