// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "screen.h"

#include "capabilities.h"
#include "engine.h"
#include "options.h"
#include "os.h"

#include <iostream>

using namespace std::chrono_literals;

screen::screen(const capabilities& caps, const options& options)
    : _caps{caps}, _using_colors{options.color && caps.has_color},
      _using_sound{options.sound && caps.has_macros},
      _blink_allowed{options.blink}, _fps{options.fps},
      _keyboard_thread{&screen::_key_reader, this}
{
    _ri = caps.has_8bit ? "\215" : "\033M";
    _dcs = caps.has_8bit ? "\220" : "\033P";
    _csi = caps.has_8bit ? "\233" : "\033[";
    _st = caps.has_8bit ? "\234" : "\033\\";
    _y_indent = std::max((caps.height - engine::height) / 2, 0);
    _x_indent = std::max((caps.width - engine::width) / 4 * 2, 0);
    _clear_macros();
    reset();
}

bool screen::blink_allowed() const
{
    return _blink_allowed;
}

void screen::reset()
{
    _last_y = -1;
    _last_x = -1;
    _last_color = color::unknown;
    _sgr(color::white);
    _write(_csi, "999;999H");
    _write(_csi, "1J");
    wait_for_terminal();
}

void screen::clear_line(const int y)
{
    _cup(y, 1);
    _write(_csi, 'K');
}

void screen::write(const char c)
{
    _write(c);
    _last_x++;
}

void screen::write(const std::string_view s)
{
    for (auto c : s)
        write(c);
}

void screen::write(const int y, const int x, const char c, const color color)
{
    _sgr(color);
    _cup(y, x);
    _write(c);
    _last_x++;
}

void screen::write(const int y, const int x, const std::string_view s, const color color)
{
    _sgr(color);
    _cup(y, x);
    for (auto c : s) {
        _write(c);
        _last_x++;
    }
}

void screen::fill_color(const int top, const int left, const int bottom, const int right, const color color)
{
    if (_using_colors && _caps.has_rectangle_ops) {
        const auto abs_top = top + _y_indent;
        const auto abs_left = left + _x_indent;
        const auto abs_bottom = bottom + _y_indent;
        const auto abs_right = right + _x_indent;
        const auto attrs = 30 + (int(color) & 7);
        _write(_csi, abs_top, ';', abs_left, ';', abs_bottom, ';', abs_right, ";0;", attrs, "$r");
    }
}

void screen::set_palette(const color color, const std::string_view rgb)
{
    // VTStar can't handle DECCTR and will echo the palette to the screen, so
    // even though it supports color, which shouldn't attempt palette changes.
    if (_using_colors && _caps.terminal_id != 66)
        _write(_dcs, "2$p", int(color), ";2;", rgb, _st);
}

void screen::set_charset(const std::string_view id)
{
    _write("\033(", id);
}

void screen::play_sound(const int pitch)
{
    if (_using_sound)
        _write(_csi, "4;1;", pitch, ",~");
}

void screen::pause(const std::chrono::milliseconds milliseconds)
{
    flush();
    if (!_exit_requested)
        std::this_thread::sleep_for(milliseconds);
}

void screen::flush()
{
    if (_buffer_index) {
        if (!_exit_requested) {
            std::cout.write(&_buffer[0], _buffer_index);
            std::cout.flush();
        }
        _buffer_index = 0;
    }
}

void screen::shutdown_keyboard()
{
    _keyboard_shutdown = true;
    _keyboard_thread.join();
}

void screen::wait_for_terminal()
{
    auto lock = std::unique_lock{_cpr_mutex};
    if (!_exit_requested) {
        _cpr_received = false;
        _write(_csi, "6n");
        flush();
        // max_used = 0;
        _cpr_condition.wait(lock, [this] { return _cpr_received; });
    }
}

void screen::reset_keys()
{
    _key_pressed = key::none;
}

key screen::key_pressed() const
{
    return _key_pressed;
}

bool screen::exit_requested() const
{
    return _exit_requested;
}

void screen::invoke_macro(const std::string macro)
{
    _write(macro.c_str());
}

void screen::_sgr(const color color)
{
    if (!_using_colors) {
        auto mono_color = color::mono_normal;
        if (color == color::wall)
            mono_color = color::mono_bright;
        if ((color == color::crouton_1 || color == color::crouton_2) && _blink_allowed)
            mono_color = color::mono_blinking;
        if (mono_color != _last_color) {
            _last_color = mono_color;
            switch (mono_color) {
                case color::mono_normal:
                    _write(_csi, "m");
                    break;
                case color::mono_blinking:
                    _write(_csi, "0;5m");
                    break;
                case color::mono_bright:
                    _write(_csi, "0;1m");
                    break;
            }
        }
    } else if (color != _last_color) {
        const auto bright = int(color) > 7;
        const auto last_bright = int(_last_color) > 7;
        const auto prefix = bright != last_bright ? (bright ? "1;" : ";") : "";
        _last_color = color;
        switch (color) {
            case color::red:
            case color::crouton_1:
                _write(_csi, prefix, "31m");
                break;
            case color::yellow:
                _write(_csi, prefix, "33m");
                break;
            case color::blue:
                _write(_csi, prefix, "34m");
                break;
            case color::time:
            case color::crouton_2:
                _write(_csi, prefix, "35m");
                break;
            case color::cyan:
                _write(_csi, prefix, "36m");
                break;
            case color::white:
                _write(_csi, 'm');
                break;
            case color::wall:
                _write(_csi, prefix, "37m");
                break;
        }
    }
}

void screen::_cup(const int y, const int x)
{
    const auto abs_y = y + _y_indent;
    const auto abs_x = x + _x_indent;
    const auto unknown = _last_y == -1 || _last_x == -1;
    auto diff_y = unknown ? 9999 : abs_y - _last_y;
    auto diff_x = unknown ? 9999 : abs_x - _last_x;
    if (diff_y || diff_x) {
        if (abs(diff_y) > 2 && abs(diff_x) > 2)
            _write(_csi, abs_y, ';', abs_x, 'H');
        else {
            _move_y_relative(diff_y);
            _move_x_relative(diff_x);
        }
        _last_y = abs_y;
        _last_x = abs_x;
    }
}

void screen::_move_y_relative(const int diff_y)
{
    if (diff_y == -1)
        _write(_ri);
    else if (diff_y == -2)
        _write(_ri, _ri);
    else if (diff_y == 1)
        _write('\v');
    else if (diff_y == 2)
        _write("\v\v");
    else if (diff_y > 0)
        _write(_csi, diff_y, 'B');
    else if (diff_y < 0)
        _write(_csi, -diff_y, 'A');
}

void screen::_move_x_relative(const int diff_x)
{
    if (diff_x == -1)
        _write('\b');
    else if (diff_x == -2)
        _write("\b\b");
    else if (diff_x == 1)
        _write(_csi, 'C');
    else if (diff_x > 0)
        _write(_csi, diff_x, 'C');
    else if (diff_x < 0)
        _write(_csi, -diff_x, 'D');
}

void screen::_write()
{
}

template <typename... Args>
void screen::_write(const int n, Args... args)
{
    _write(std::to_string(n));
    _write(args...);
}

template <typename... Args>
void screen::_write(const std::string_view s, Args... args)
{
    for (auto c : s)
        _write(c);
    _write(args...);
}

template <typename... Args>
void screen::_write(const char c, Args... args)
{
    _buffer[_buffer_index++] = c;
    _write(args...);
}

void screen::_key_reader()
{
    while (!_keyboard_shutdown) {
        const auto ch = os::getch();
        if (ch == 'A') {
            _key_pressed = key::up;
        } else if (ch == 'B') {
            _key_pressed = key::down;
        } else if (ch == 'C') {
            _key_pressed = key::right;
        } else if (ch == 'D') {
            _key_pressed = key::left;
        } else if (ch == 'R') {
            _notify_cpr_received();
            if (_exit_requested) break;
        } else if (ch == 'Q' || ch == 'q' || ch == 3) {
            _exit_requested = true;
            if (_cpr_received) break;
        }
    }
}

void screen::_notify_cpr_received()
{
    {
        auto lock = std::lock_guard{_cpr_mutex};
        _cpr_received = true;
    }
    _cpr_condition.notify_one();
}

std::string screen::_define_macro(const int id, const std::string_view content)
{
    if (_caps.has_macros && content.size() > 0) {
        static constexpr auto hex_digits = "0123456789ABCDEF";
        auto hex_content = std::string(content.size() * 2, ' ');
        for (auto i = 0; i < content.size(); i++) {
            hex_content[i * 2] = hex_digits[(content[i] >> 4) & 0x0F];
            hex_content[i * 2 + 1] = hex_digits[content[i] & 0x0F];
        }
        _write(_dcs, id, ";0;1!z", hex_content, _st);
        return _csi + std::to_string(id) + "*z";
    } else {
        return std::string{content};
    }
}

void screen::_clear_macros()
{
    if (_caps.has_macros)
        _write(_dcs, "0;1;0!z", _st);
}
