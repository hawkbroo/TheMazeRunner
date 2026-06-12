#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Спрайт-лист портала: 8x8, 64 кадра.
struct PortalSheet {
    sf::Texture texture;
    unsigned cellW{128};
    unsigned cellH{128};
    bool loaded{false};

    static constexpr int COLS = 8;
    static constexpr int ROWS = 8;
    static constexpr int FRAME_COUNT = 64;
    static constexpr float FRAME_TIME = 0.03f;

    bool loadFromFile(const std::string& path);
    sf::IntRect frameRect(int frame) const;
};
