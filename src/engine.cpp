// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "engine.h"

#include "coloring.h"
#include "font.h"
#include "levels.h"
#include "options.h"
#include "screen.h"
#include "snake.h"
#include "status.h"

#include <chrono>

using namespace std::chrono_literals;

engine::engine(const capabilities& caps, const options& options, soft_font& font)
    : _caps{caps}, _options{options}, _font{font}
{
}

bool engine::run()
{
    screen screen(_caps, _options);
    status status{screen};

    static auto chomp_macro = std::string{};
    static auto short_chomp_macro = std::string{};
    if (chomp_macro.empty()) {
        chomp_macro = screen.define_macro(3, [&]() {
            screen.play_sound(7);
            screen.play_sound(10);
        });
        short_chomp_macro = screen.define_macro(4, [&]() {
            screen.play_sound(10);
        });
    }

    for (auto wave = 1; !screen.exit_requested(); wave++) {
        if (wave == 100) wave = 80;
        if (wave % 4 == 0) status.gain_life();

        // In the original arcade game the speed increases almost every wave,
        // but our implementation is much simpler. The first four waves are
        // about the same as the starting arcade speed, and then from wave
        // five onwards it's 50% faster.
        const auto frames_per_move = (wave > 4 ? 2 : 3);

        _font.init(wave);
        level level{screen, wave};
        snake snake{screen, level};
        level.init_map();
        status.init(wave);
        level.init_croutons();
        snake.init();

        screen.reset_keys();
        while (!screen.exit_requested() && !level.complete()) {
            auto reset_key = false;
            switch (screen.key_pressed()) {
                case key::up:
                    reset_key = snake.turn(-1, 0);
                    break;
                case key::down:
                    reset_key = snake.turn(+1, 0);
                    break;
                case key::right:
                    reset_key = snake.turn(0, +1);
                    break;
                case key::left:
                    reset_key = snake.turn(0, -1);
                    break;
            }
            if (reset_key) screen.reset_keys();

            snake.move();
            const auto [snake_y, snake_x] = snake.position();
            if (level.eat_crouton(snake_y, snake_x)) {
                status.add_points(level.points_per_crouton());
                snake.grow();
            }

            status.update(frames_per_move);
            level.update(frames_per_move);

            if (snake.is_dead() || status.out_of_time()) {
                status.lose_life();
                if (snake.erase()) {
                    screen.reset();
                    screen.set_palette(color::snake, palette::bright_red);
                    if (status.out_of_time()) {
                        status.init(wave);
                        _display_time_out(screen);
                        screen.reset();
                    }
                    if (status.game_over()) {
                        status.init(wave);
                        _display_game_over(screen);
                        break;
                    }
                    status.reset_time();
                    status.init(wave);
                    level.init_map();
                    level.init_croutons();
                    snake.init();
                    screen.reset_keys();
                }
            }

            // We're stretching the definition of a second here so the default
            // frame rate is slow enough to support the two-note chomp sound.
            const auto time_between_moves = frames_per_move * 1050ms / _options.fps;
            if (snake.just_eaten()) {
                if (time_between_moves >= 62ms)
                    screen.invoke_macro(chomp_macro);
                else if (time_between_moves >= 31ms)
                    screen.invoke_macro(short_chomp_macro);
            }
            screen.pause(time_between_moves);
        }

        if (status.game_over()) {
            break;
        } else if (level.complete()) {
            status.apply_bonus();
            status.reset_time();
            screen.reset();
        }
    }

    screen.shutdown_keyboard();
    return !screen.exit_requested();
}

void engine::_display_time_out(screen& screen)
{
    screen.set_charset("B");
    screen.write(11, 9, "", color::yellow);
    for (auto ch : "NIBBLER RAN OUT OF TIME") {
        screen.write(ch);
        screen.pause(66ms);
    }
    screen.set_charset(" @");
    screen.pause(1s);
}

void engine::_display_game_over(screen& screen)
{
    screen.set_charset("B");
    screen.write(12, 16, "GAME OVER", color::red);
    screen.set_charset(" @");
    screen.flush();
}
