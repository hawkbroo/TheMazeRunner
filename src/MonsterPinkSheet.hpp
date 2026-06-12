#pragma once

#include <SFML/Graphics.hpp>

// Розовый монстр: 6 колонок x 3 ряда.
// Ряды: run (6) | transform (5) | kill (5).
struct MonsterPinkSheet {
    sf::Texture texture;
    unsigned cellW{70};
    unsigned cellH{100};
    bool loaded{false};

    static constexpr int COLS = 6;
    static constexpr int ROWS = 3;

    static constexpr int ROW_RUN = 0;
    static constexpr int ROW_TRANSFORM = 1;
    static constexpr int ROW_KILL = 2;

    static constexpr int RUN_FRAMES = 6;
    static constexpr int TRANSFORM_FRAMES = 5;
    static constexpr int KILL_FRAMES = 5;
    // Последний кадр на листе — монстр «падает»; в игре не показываем.
    static constexpr int KILL_PLAY_FRAMES = 4;

    bool loadFromFile(const std::string& path);
    sf::IntRect cellRect(int col, int row) const;
};
