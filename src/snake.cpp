// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "snake.h"

#include "levels.h"
#include "screen.h"

using namespace std::literals;

namespace {

    constexpr auto grid_sprite = "XZZZZZZZZZZZZ"sv;

    constexpr auto head_down_sprites = std::to_array({"lmJK"sv, "jk\"#"sv, "noJK"sv, "ef$%"sv});
    constexpr auto head_up_sprites = std::to_array({"rsNQ"sv, "jk)*"sv, "pqNQ"sv, "gh&("sv});
    constexpr auto head_right_sprites = std::to_array({"ml"sv, "ii"sv, "sr"sv, "ab"sv});
    constexpr auto head_left_sprites = std::to_array({"no"sv, "ii"sv, "pq"sv, "cd"sv});

    constexpr auto tail_down_sprites = std::to_array({"pqtu"sv, "-@xy"sv, "rsvw"sv, "+?  "sv});
    constexpr auto tail_up_sprites = std::to_array({"notu"sv, "[]zU"sv, "lmvw"sv, "^_  "sv});
    constexpr auto tail_right_sprites = std::to_array({"vpzU"sv, "vwvw"sv, "vnxy"sv});
    constexpr auto tail_left_sprites = std::to_array({"suzU"sv, "tutu"sv, "muxy"sv});

    constexpr auto snake_erase_palette = std::to_array({
        palette::white,
        palette::gray,
        palette::bright_yellow,
        palette::bright_red,
        palette::blue,
        palette::purple,
        palette::bright_yellow,
        palette::purple,
    });

    constexpr auto text_erase_palette = std::to_array({
        palette::purple,
        palette::cyan,
        palette::blue,
        palette::white,
        palette::gray,
        palette::bright_yellow,
        palette::bright_red,
        palette::bright_yellow,
    });

    int sign(const int n)
    {
        return (n > 0) - (n < 0);
    }

}  // namespace

snake::snake(screen& screen, const level& level)
    : _screen{screen}, _level{level}
{
}

void snake::init()
{
    const auto y = 32;
    const auto x = 10;

    _body.clear();
    for (auto i = 0; i < 11; i++)
        _body.emplace_back(y, x + i);

    std::fill(_occupied.begin(), _occupied.end(), false);

    auto snake_sprite = std::string{};
    snake_sprite += tail_right_sprites[1].substr(0, 2);
    snake_sprite += std::string(8, head_right_sprites[1][0]);
    snake_sprite += head_right_sprites[3];
    snake_sprite += ' ';

    const auto slow_render = [&](const std::string_view sprite, const auto color, int pitch) {
        _render(y, x, "", color);
        for (auto c : sprite) {
            _screen.write(c);
            _screen.play_sound(pitch++);
            _screen.pause(32ms);
        }
    };
    slow_render(grid_sprite, color::yellow, 1);
    _screen.pause(200ms);
    slow_render(snake_sprite, color::blue, 4);
    _screen.pause(200ms);
    slow_render(snake_sprite, color::snake, 8);
    _screen.wait_for_terminal();

    _dy = 0;
    _dx = 1;
    _paused = 0;
    _growing = 0;
    _just_eaten = false;
    _dead = false;
}

void snake::move()
{
    _just_eaten = false;
    if (_can_move(_dy, _dx)) {
        auto& head = _body.back();
        _dead = _is_occupied(head.y + _dy * 2, head.x + _dx * 2);
        if (_dead) return;
        _body.emplace_back(head.y + _dy, head.x + _dx);
        _render_head();
        if (_growing > 0)
            _growing--;
        else {
            _render_tail();
            _body.erase(_body.cbegin());
            _paused = 0;
        }
        _screen.flush();
    } else if (++_paused == 5) {
        static constexpr auto dx = std::to_array({0, 0, -1, 1});
        static constexpr auto dy = std::to_array({-1, 1, 0, 0});
        auto dir = -1;
        for (auto i = 0; i < 4; i++) {
            if (_can_move(dy[i], dx[i])) {
                dir = (dir < 0 ? i : -1);
                if (dir < 0) break;
            }
        }
        if (dir >= 0) {
            _dy = dy[dir];
            _dx = dx[dir];
            move();
        }
    }
}

bool snake::turn(const int dy, const int dx)
{
    const auto& head = _body.back();
    if ((head.y % 2) == 0 && (head.x % 2) == 0 && _can_move(dy, dx)) {
        _dy = dy;
        _dx = dx;
        return true;
    }
    return false;
}

void snake::grow()
{
    _growing = 3;
    _just_eaten = true;
}

bool snake::erase()
{
    _screen.set_palette(color::snake, snake_erase_palette[0]);
    for (auto i = 1; !_screen.exit_requested(); i++) {
        if (i < 26) _screen.play_sound(26 - i);
        _screen.pause(32ms);
        if (_body.empty()) break;
        if (i % 3 == 0 && _screen.blink_allowed()) {
            const auto palette_index = i / 3 % snake_erase_palette.size();
            _screen.set_palette(color::snake, snake_erase_palette[palette_index]);
            _screen.set_palette(color::text, text_erase_palette[palette_index]);
        }
        const auto& tail = _body.front();
        if (_body.size() == 1)
            _render(tail.y, tail.x, "  ");
        else {
            const auto& next_tail = _body[1];
            const auto dy = next_tail.y - tail.y;
            const auto dx = next_tail.x - tail.x;
            if (dx)
                _render(tail.y, tail.x + (dx < 0), " ");
            else if (tail.y % 2)
                _render(tail.y + (dy < 0), tail.x, "  ");
        }
        _body.erase(_body.cbegin());
    }
    return !_screen.exit_requested();
}

std::tuple<int, int> snake::position() const
{
    const auto& head = _body.back();
    return {head.y, head.x};
}

bool snake::just_eaten() const
{
    return _just_eaten;
}

bool snake::is_dead() const
{
    return _dead;
}

bool snake::_can_move(const int dy, const int dx) const
{
    const auto& head = _body.back();
    const auto next_y = head.y + dy;
    const auto next_x = head.x + dx;
    const auto clear1 = _level.is_path(next_y, next_x);
    const auto clear2 = _level.is_path(next_y + 1, next_x + 1);
    const auto reversing = dx * _dx < 0 || dy * _dy < 0;
    return clear1 && clear2 && !reversing;
}

void snake::_render_head()
{
    const auto& head = _body.back();
    const auto& last_head = _body[_body.size() - 2];
    const auto& further_back = _body[_body.size() - 4];
    const auto dy = head.y - last_head.y;
    const auto dx = head.x - last_head.x;
    if (dy) {
        const auto turn = sign(head.x - further_back.x) + 1;
        const auto sprite_offset = (head.y % 2) * 2;
        if (dy > 0) {  // facing down
            _render(head.y - 1, head.x, head_down_sprites[turn].substr(sprite_offset, 2));
            _render(head.y + 1, head.x, head_down_sprites[3].substr(sprite_offset, 2));
        } else {  // facing up
            _render(head.y, head.x, head_up_sprites[3].substr(sprite_offset, 2));
            _render(head.y + 2, head.x, head_up_sprites[turn].substr(sprite_offset, 2));
        }
    } else {
        const auto turn = sign(head.y - further_back.y) + 1;
        const auto sprite_offset = (head.x % 2);
        if (dx > 0) {  // facing right
            _render(head.y, head.x - 1, head_right_sprites[turn].substr(sprite_offset, 1));
            _render(head.y, head.x, head_right_sprites[3]);
        } else {  // facing left
            _render(head.y, head.x, head_left_sprites[3]);
            _render(head.y, head.x + 2, head_left_sprites[turn].substr(sprite_offset, 1));
        }
    }
    if (head.y % 2 == 0 && head.x % 2 == 0)
        _track_occupation(head.y, head.x, true);
}

void snake::_render_tail()
{
    const auto& tail = _body.front();
    const auto& next_tail = _body[1];
    const auto& further_forward = _body[3];
    const auto dy = next_tail.y - tail.y;
    const auto dx = next_tail.x - tail.x;
    if (dy) {
        const auto turn = sign(further_forward.x - tail.x) + 1;
        const auto sprite_offset = (tail.y % 2) * 2;
        if (dy > 0) {  // facing down
            _render(tail.y, tail.x, tail_down_sprites[3].substr(sprite_offset, 2));
            _render(tail.y + 2, tail.x, tail_down_sprites[turn].substr(sprite_offset, 2));
        } else {  // facing up
            _render(tail.y - 1, tail.x, tail_up_sprites[turn].substr(sprite_offset, 2));
            _render(tail.y + 1, tail.x, tail_up_sprites[3].substr(sprite_offset, 2));
        }
    } else {
        const auto turn = sign(further_forward.y - tail.y) + 1;
        const auto sprite_offset = (tail.x % 2) * 2;
        if (dx > 0) {  // facing right
            _render(tail.y, tail.x, " ");
            _render(tail.y, tail.x + 1, tail_right_sprites[turn].substr(sprite_offset, 2));
        } else {  // facing left
            _render(tail.y, tail.x - 1, tail_left_sprites[turn].substr(sprite_offset, 2));
            _render(tail.y, tail.x + 1, " ");
        }
    }
    if (tail.y % 2 != 0 || tail.x % 2 != 0)
        _track_occupation(tail.y - dy, tail.x - dx, false);
}

void snake::_render(const int y, const int x, const std::string_view s, const color color)
{
    const auto sy = 4 + y / 2;
    const auto sx = 3 + x;
    _screen.write(sy, sx, s, color);
}

void snake::_track_occupation(const int y, const int x, const bool occupied)
{
    _occupied[y / 2 * 17 + x / 2] = occupied;
}

bool snake::_is_occupied(const int y, const int x) const
{
    if (y % 2 != 0 || x % 2 != 0) return false;
    return _occupied[y / 2 * 17 + x / 2];
}
