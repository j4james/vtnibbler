// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include "coloring.h"

#include <array>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

class capabilities;
class options;

enum class key {
    none,
    left,
    right,
    up,
    down
};

class screen {
public:
    screen(const capabilities& caps, const options& options);
    bool blink_allowed() const;
    void reset();
    void clear_line(const int y);
    void write(const char c);
    void write(const std::string_view s);
    void write(const int y, const int x, const char c, const color color);
    void write(const int y, const int x, const std::string_view s, const color color);
    void fill_color(const int top, const int left, const int bottom, const int right, const color color);
    void set_palette(const color color, const std::string_view rgb);
    void set_charset(const std::string_view id);
    void play_sound(const int pitch);
    void pause(const std::chrono::milliseconds milliseconds);
    void flush();

    void shutdown_keyboard();
    void wait_for_terminal();
    void reset_keys();
    key key_pressed() const;
    bool exit_requested() const;

    template <typename T>
    std::string define_macro(const int id, T&& lambda);
    void invoke_macro(const std::string macro);

private:
    void _sgr(const color color);
    void _cup(const int y, const int x);
    void _move_y_relative(const int diff_y);
    void _move_x_relative(const int diff_y);
    void _write();
    template <typename... Args>
    void _write(const int n, Args... args);
    template <typename... Args>
    void _write(const std::string_view s, Args... args);
    template <typename... Args>
    void _write(const char c, Args... args);
    void _key_reader();
    void _notify_cpr_received();
    std::string _define_macro(const int id, const std::string_view content);
    void _clear_macros();

    const capabilities& _caps;
    const bool _using_colors;
    const bool _using_sound;
    const bool _blink_allowed;
    const int _fps;
    const char* _ri;
    const char* _dcs;
    const char* _csi;
    const char* _st;
    int _y_indent;
    int _x_indent;
    int _last_y = -1;
    int _last_x = -1;
    color _last_color = color::unknown;
    std::array<char, 512> _buffer = {};
    int _buffer_index = 0;

    volatile key _key_pressed = key::none;
    volatile bool _keyboard_shutdown = false;
    volatile bool _exit_requested = false;
    std::thread _keyboard_thread;
    bool _cpr_received = true;
    std::condition_variable _cpr_condition;
    std::mutex _cpr_mutex;
};

template <typename T>
std::string screen::define_macro(const int id, T&& lambda)
{
    const auto start_index = _buffer_index;
    lambda();
    const auto length = static_cast<size_t>(_buffer_index - start_index);
    _buffer_index = start_index;
    return _define_macro(id, {&_buffer[start_index], length});
}
