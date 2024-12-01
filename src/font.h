// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

class capabilities;

class soft_font {
public:
    enum size {
        size_8x10,
        size_15x12,
        size_10x20,
        size_12x30,
        size_10x16
    };

    soft_font(const capabilities& caps);
    ~soft_font();
    void init(const int wave);

private:
    const size _font_size;
};
