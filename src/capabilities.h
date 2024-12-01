// VT Nibbler
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include <optional>
#include <regex>
#include <string>
#include <string_view>

class capabilities {
public:
    capabilities();
    std::optional<bool> query_mode(const int mode) const;
    std::string query_setting(const std::string_view setting) const;
    std::string query_color_table() const;

    int width = 80;
    int height = 24;
    bool has_soft_fonts = false;
    bool has_color = false;
    bool has_rectangle_ops = false;
    bool has_macros = false;
    bool has_8bit = false;
    int terminal_id = 0;

private:
    void _query_device_attributes();
    void _query_terminal_id();
    static std::smatch _query(const char* pattern, const bool may_not_work);
};
