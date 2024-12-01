// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

class screen;

class status {
public:
    status(screen& screen);
    ~status();
    void init(const int wave);
    void update(const int elapsed_frames);
    void add_points(const int points);
    void lose_life();
    void gain_life();
    void apply_bonus();
    void reset_time();
    bool out_of_time() const;
    bool game_over() const;

private:
    void _render_score();
    void _render_high_score();
    void _render_lives();
    void _render_time();
    void _render_wave(const int wave);

    screen& _screen;
    static int _high_score;
    int _score = 0;
    int _lives = 3;
    int _time = 990;
    int _frame = 0;
    int _last_score_frame = 0;
};
