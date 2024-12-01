// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "levels.h"

#include "coloring.h"
#include "screen.h"

#include <span>

namespace {

    constexpr auto map_1 = std::to_array({
        "lqwwqwwqk",
        "x xtwux x",
        "tqjxxxmqu",
        "xlqvnvqkx",
        "tvqwvwqvu",
        "tqwu twqu",
        "x xtqux x",
        "tqjx xmqu",
        "mqqvqvqqj",
    });

    constexpr auto map_2 = std::to_array({
        "lqqwwwqqk",
        "tqwjxmwqu",
        "x tqvqu x",
        "mwvqwqvwj",
        "lvqqnqqvk",
        "tqqkxlqqu",
        "twktnulwu",
        "xxmjxmjxx",
        "mvqqvqqvj",
    });

    constexpr auto map_3 = std::to_array({
        "lwwwwwwwk",
        "tnnnnnnnu",
        "tnnnnnnnu",
        "tnnnnnnnu",
        "tnnnnnnnu",
        "tnnnnnnnu",
        "tnnnnnnnu",
        "tnnnnnnnu",
        "mvvvvvvvj",
    });

    constexpr auto map_4 = std::to_array({
        "lqwwqwwqk",
        "tqutqutqu",
        "xlvnqnvkx",
        "tulu tktu",
        "tnutqutnu",
        "tumu tjtu",
        "xmwnqnwjx",
        "tqutqutqu",
        "mqvvqvvqj",
    });

    constexpr auto map_11 = std::to_array({
        "lklwqwklk",
        "xtnulnnux",
        "tjtvnvumu",
        "mwvwvkmwj",
        " twu twu ",
        "lnumwjtnk",
        "mnnwnwnnj",
        "lutumnjtk",
        "mvvvqvqvj",
    });

    constexpr auto map_13 = std::to_array({
        "lwqwwwqwk",
        "tjluxtkmu",
        "tquxxxtqu",
        "xlvjxmvkx",
        "tnqqnqqnu",
        "xmwkxlwjx",
        "tquxxxtqu",
        "tkmuxtjlu",
        "mvqvvvqvj",
    });

    constexpr auto map_15 = std::to_array({
        "lqqwwwqqk",
        "tqwjxmwqu",
        "x tqvqu x",
        "mqvqwqvqj",
        "lqqqnqqqk",
        "tqqkxlqqu",
        "twktnulwu",
        "xxmjxmjxx",
        "mvqqvqqvj",
    });

    constexpr auto map_16 = std::to_array({
        "lwwwqwwwk",
        "tjxtquxmu",
        "twvnwnvwu",
        "xtquxtqux",
        "tvwnvnwvu",
        "tkxtquxlu",
        "tnvnwnvnu",
        "xtquxtqux",
        "mvqvvvqvj",
    });

    constexpr auto croutons_1 = std::to_array({2, 6, 10, 14, 34, 38, 46, 50, 74, 78, 102, 105, 115, 118, 127, 139, 149, 172, 184, 212, 221, 225, 227, 231, 233, 237, 255, 271});
    constexpr auto croutons_4 = std::to_array({2, 8, 14, 23, 27, 36, 42, 48, 68, 70, 76, 82, 84, 91, 95, 140, 144, 148, 193, 197, 204, 212, 220, 225, 233, 240, 246, 252});
    constexpr auto croutons_7 = std::to_array({2, 6, 10, 14, 38, 46, 74, 78, 102, 105, 115, 118, 127, 139, 149, 159, 163, 172, 184, 212, 221, 225, 227, 231, 233, 237, 255, 271});
    constexpr auto croutons_8 = std::to_array({2, 6, 10, 14, 25, 34, 38, 46, 50, 74, 78, 102, 105, 115, 118, 127, 139, 149, 172, 184, 212, 221, 225, 227, 231, 233, 237, 255, 271});
    constexpr auto croutons_11 = std::to_array({0, 4, 12, 16, 40, 42, 70, 78, 80, 82, 102, 107, 112, 118, 157, 165, 170, 176, 180, 186, 204, 212, 220, 244, 250, 257, 269, 272, 288});
    constexpr auto croutons_12 = std::to_array({23, 27, 35, 38, 42, 46, 49, 72, 74, 78, 80, 85, 101, 121, 133, 161, 175, 181, 212, 238, 240, 242, 250, 252, 254, 263});
    constexpr auto croutons_13 = std::to_array({36, 38, 46, 48, 51, 59, 67, 72, 80, 104, 108, 112, 116, 139, 144, 149, 172, 176, 180, 184, 208, 216, 221, 229, 237, 240, 242, 250, 252});
    constexpr auto croutons_16 = std::to_array({23, 27, 36, 38, 42, 46, 48, 74, 78, 106, 110, 114, 121, 133, 142, 146, 172, 174, 178, 182, 184, 210, 214, 223, 235, 242, 246, 250});
    constexpr auto croutons_17 = std::to_array({6, 10, 17, 19, 31, 33, 51, 53, 65, 67, 74, 78, 93, 108, 112, 138, 142, 146, 170, 176, 180, 182, 186, 195, 242, 244, 250, 255, 265});
    constexpr auto croutons_20 = std::to_array({2, 8, 14, 36, 42, 48, 55, 63, 76, 103, 117, 123, 131, 137, 142, 146, 151, 157, 165, 171, 185, 212, 225, 233, 240, 246, 252});
    constexpr auto croutons_30 = std::to_array({8, 35, 39, 45, 49, 53, 57, 61, 65, 71, 81, 93, 123, 131, 136, 144, 152, 157, 165, 195, 207, 217, 223, 227, 231, 235, 239, 243, 249, 253});

    const auto level_number(const int wave)
    {
        return (wave - 1) % 32 + 1;
    }

    const auto& map_for_wave(const int wave)
    {
        switch (level_number(wave)) {
            case 1: return map_1;
            case 2: return map_2;
            case 3: return map_3;
            case 4: return map_4;
            case 5: return map_2;
            case 6: return map_3;
            case 7: return map_1;
            case 8: return map_2;
            case 9: return map_3;
            case 10: return map_4;
            case 11: return map_11;
            case 12: return map_2;
            case 13: return map_13;
            case 14: return map_1;
            case 15: return map_15;
            case 16: return map_16;
            case 17: return map_11;
            case 18: return map_3;
            case 19: return map_13;
            case 20: return map_4;
            case 21: return map_11;
            case 22: return map_16;
            case 23: return map_15;
            case 24: return map_1;
            case 25: return map_13;
            case 26: return map_11;
            case 27: return map_16;
            case 28: return map_4;
            case 29: return map_15;
            case 30: return map_3;
            case 31: return map_11;
            case 32: return map_16;
            default: return map_1;
        }
    }

    const auto map_palette_for_wave(const int wave)
    {
        switch (level_number(wave)) {
            case 1: return palette::white_blue;
            case 2: return palette::red_purple;
            case 3: return palette::red;
            case 4: return palette::green_blue;
            case 5: return palette::red_orange;
            case 6: return palette::purple;
            case 7: return palette::green_red;
            case 8: return palette::red_purple;
            case 9: return palette::purple;
            case 10: return palette::blue_yellow;
            case 11: return palette::red_purple;
            case 12: return palette::green_blue;
            case 13: return palette::cyan_red;
            case 14: return palette::purple_orange;
            case 15: return palette::red_orange;
            case 16: return palette::blue_yellow;
            case 17: return palette::green_red;
            case 18: return palette::cyan_red;
            case 19: return palette::red_purple;
            case 20: return palette::green_blue;
            case 21: return palette::red_purple;
            case 22: return palette::purple_orange;
            case 23: return palette::red_orange;
            case 24: return palette::red_purple;
            case 25: return palette::green_red;
            case 26: return palette::green_blue;
            case 27: return palette::purple_orange;
            case 28: return palette::cyan_red;
            case 29: return palette::purple_orange;
            case 30: return palette::red;
            case 31: return palette::blue_yellow;
            case 32: return palette::green_red;
            default: return palette::white;
        }
    }


    const std::span<const int> croutons_for_wave(const int wave)
    {
        switch (level_number(wave)) {
            case 1: return croutons_1;
            case 2: return croutons_1;
            case 3: return croutons_1;
            case 4: return croutons_4;
            case 5: return croutons_1;
            case 6: return croutons_1;
            case 7: return croutons_7;
            case 8: return croutons_8;
            case 9: return croutons_1;
            case 10: return croutons_4;
            case 11: return croutons_11;
            case 12: return croutons_12;
            case 13: return croutons_13;
            case 14: return croutons_7;
            case 15: return croutons_1;
            case 16: return croutons_16;
            case 17: return croutons_17;
            case 18: return croutons_4;
            case 19: return croutons_13;
            case 20: return croutons_20;
            case 21: return croutons_11;
            case 22: return croutons_16;
            case 23: return croutons_1;
            case 24: return croutons_7;
            case 25: return croutons_13;
            case 26: return croutons_17;
            case 27: return croutons_16;
            case 28: return croutons_20;
            case 29: return croutons_1;
            case 30: return croutons_30;
            case 31: return croutons_17;
            case 32: return croutons_16;
            default: return croutons_1;
        }
    }

    const std::array<const char*, 2> crouton_palette_for_wave(const int wave)
    {
        switch (level_number(wave)) {
            case 1: return {palette::bright_green, palette::bright_purple};
            case 2: return {palette::orange, palette::white};
            case 3: return {palette::white, palette::bright_purple};
            case 4: return {palette::bright_purple, palette::bright_yellow};
            case 5: return {palette::bright_yellow, palette::bright_cyan};
            case 6: return {palette::green, palette::red};
            case 7: return {palette::red, palette::orange};
            case 8: return {palette::orange, palette::white};
            case 9: return {palette::bright_purple, palette::orange};
            case 10: return {palette::orange, palette::purple};
            case 11: return {palette::purple, palette::green};
            case 12: return {palette::bright_green, palette::bright_purple};
            case 13: return {palette::bright_purple, palette::white};
            case 14: return {palette::red, palette::orange};
            case 15: return {palette::bright_purple, palette::blue};
            case 16: return {palette::blue, palette::white};
            case 17: return {palette::orange, palette::purple};
            case 18: return {palette::bright_yellow, palette::bright_yellow};
            case 19: return {palette::green, palette::purple};
            case 20: return {palette::bright_yellow, palette::bright_cyan};
            case 21: return {palette::purple, palette::bright_yellow};
            case 22: return {palette::blue, palette::orange};
            case 23: return {palette::bright_red, palette::blue};
            case 24: return {palette::bright_purple, palette::white};
            case 25: return {palette::bright_green, palette::bright_purple};
            case 26: return {palette::bright_purple, palette::bright_yellow};
            case 27: return {palette::bright_yellow, palette::bright_cyan};
            case 28: return {palette::bright_purple, palette::bright_yellow};
            case 29: return {palette::red, palette::orange};
            case 30: return {palette::orange, palette::purple};
            case 31: return {palette::red, palette::red};
            case 32: return {palette::purple, palette::bright_yellow};
            default: return {palette::white, palette::white};
        }
    }

    constexpr auto crouton_sprite = "{}";
    constexpr auto wall_sprites = std::to_array({
        "  ",
        "|D",
        "G!",
        "|!",
        ":;",
        ",;",
        ":.",
        ",.",
        "/\\",
        "`\\",
        "/'",
        "`'",
        "==",
        "<=",
        "=>",
        "<>",
    });

}  // namespace

level::level(screen& screen, const int wave)
    : _screen{screen}, _wave{wave}
{
    _build_map();
    _build_croutons();
    _build_palette();
}

int level::points_per_crouton() const
{
    return _wave * 10;
}

void level::init_map()
{
    _screen.set_palette(color::wall, map_palette_for_wave(_wave));
    for (auto y = 0; y < 19; y++) {
        _screen.write(3 + y, 1, "", color::wall);
        for (auto x = 0; x < 19; x++) {
            const auto shape = _wall_shape(y, x);
            _screen.write(wall_sprites[shape]);
        }
        _screen.flush();
    }
}

void level::init_croutons()
{
    const auto crouton_palette = crouton_palette_for_wave(_wave);
    _screen.set_palette(color::crouton_1, crouton_palette[0]);
    _screen.set_palette(color::crouton_2, crouton_palette[1]);
    for (auto i = 0; i < _croutons.size(); i++) {
        if (_croutons[i]) {
            const auto y = 4 + (i / 17);
            const auto x = 3 + (i % 17) * 2;
            _screen.write(y, x, crouton_sprite[0], color::crouton_1);
            _screen.write(y, x + 1, crouton_sprite[1], color::crouton_2);
            _screen.flush();
        }
    }
    _frame = 0;
    _screen.wait_for_terminal();
}

void level::update(const int elapsed_frames)
{
    constexpr auto blink_rate = 16;
    const auto last_frame = _frame;
    _frame += elapsed_frames;
    if (_frame / blink_rate > last_frame / blink_rate && _screen.blink_allowed()) {
        const auto tick = _frame / blink_rate;
        _screen.invoke_macro(tick % 2 == 0 ? _palette_macro_1 : _palette_macro_2);
    }
}

bool level::is_path(const int snake_y, const int snake_x) const
{
    return _is_path((snake_y >> 1) + 1, (snake_x >> 1) + 1);
}

bool level::eat_crouton(const int snake_y, const int snake_x)
{
    if (snake_y % 2 || snake_x % 2) return false;
    auto& crouton = _croutons[(snake_y >> 1) * 17 + (snake_x >> 1)];
    if (!crouton) return false;
    crouton = false;
    _croutons_remaining--;
    return true;
}

bool level::complete() const
{
    return _croutons_remaining <= 0;
}

void level::_build_map()
{
    static constexpr auto exit_table = std::to_array({5, 9, 10, 6, 15, 3, 3, 3, 3, 3, 14, 13, 7, 11, 12});
    const auto& map = map_for_wave(_wave);
    const auto exits = [&](const auto y, const auto x) {
        if (x < 1 && y < 1) return 0b1010;
        if (x < 1 && y > 9) return 0b0110;
        if (x < 1 || x > 9) return 0b1100;
        if (y < 1 || y > 9) return 0b0011;
        const auto c = map[y - 1][x - 1];
        return c == ' ' ? 1 : exit_table[c - 'j'];
    };
    const auto test_if_path = [&](const auto y, const auto x) {
        if ((y % 2) && (x % 2))
            return false;
        else if (y % 2)
            return (exits(y / 2, x / 2) & 8) != 0;
        else if (x % 2)
            return (exits(y / 2, x / 2) & 2) != 0;
        else
            return exits(y / 2, x / 2) != 0;
    };
    for (auto y = -1; y < 20; y++) {
        for (auto x = -1; x < 20; x++) {
            _is_path(y, x) = test_if_path(y + 1, x + 1);
        }
    }
}

void level::_build_croutons()
{
    const auto& croutons = croutons_for_wave(_wave);
    for (auto crouton : croutons)
        _croutons[crouton] = true;
    _croutons_remaining = croutons.size();
}

void level::_build_palette()
{
    if (_screen.blink_allowed()) {
        constexpr auto time_palette = std::to_array({palette::bright_yellow, palette::blue});
        const auto crouton_palette = crouton_palette_for_wave(_wave);
        _palette_macro_1 = _screen.define_macro(1, [&]() {
            _screen.set_palette(color::time, time_palette[0]);
            _screen.set_palette(color::crouton_1, crouton_palette[0]);
            _screen.set_palette(color::crouton_2, crouton_palette[1]);
        });
        _palette_macro_2 = _screen.define_macro(2, [&]() {
            _screen.set_palette(color::time, time_palette[1]);
            _screen.set_palette(color::crouton_1, crouton_palette[1]);
            _screen.set_palette(color::crouton_2, crouton_palette[0]);
        });
    }
}

int level::_wall_shape(const int y, const int x) const
{
    if (_is_path(y, x)) return 0;
    const auto left = _is_path(y + 0, x - 1);
    const auto right = _is_path(y + 0, x + 1);
    const auto top = _is_path(y - 1, x + 0);
    const auto bottom = _is_path(y + 1, x + 0);
    return left + (right << 1) + (top << 2) + (bottom << 3);
}

bool level::_is_path(const int y, const int x) const
{
    return _path[(y + 1) * 21 + (x + 1)];
}

bool& level::_is_path(const int y, const int x)
{
    return _path[(y + 1) * 21 + (x + 1)];
}
