#pragma once

#include <SFML/Graphics.hpp>

// Спрайт-лист игрока: 8 направлений x 7 рядов анимации.
struct PlayerSheet {
    sf::Texture texture;
    unsigned cellW{104};
    unsigned cellH{168};
    bool loaded{false};

    static constexpr int COLS = 8;
    static constexpr int ROWS = 7;

    static constexpr int COL_BACK = 0;
    static constexpr int COL_BACK_RIGHT = 1;
    static constexpr int COL_RIGHT = 2;
    static constexpr int COL_FRONT_RIGHT = 3;
    static constexpr int COL_FRONT = 4;
    static constexpr int COL_FRONT_LEFT = 5;
    static constexpr int COL_LEFT = 6;
    static constexpr int COL_BACK_LEFT = 7;

    static constexpr int ROW_IDLE = 0;
    static constexpr int ROW_RUN_A = 1;
    static constexpr int ROW_RUN_B = 2;
    static constexpr int ROW_FALL = 3;
    static constexpr int ROW_DEAD_BACK = 4;
    static constexpr int ROW_DEAD_DOWN_A = 5;
    static constexpr int ROW_DEAD_DOWN_B = 6;

    static constexpr int DEATH_FRAMES = 4;

    bool loadFromFile(const std::string& path);
    sf::IntRect cellRect(int col, int row) const;
};
