#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "Constants.hpp"

class Maze {
public:
    bool loadFromLayout(const std::vector<std::string>& layout);

    int width() const { return static_cast<int>(grid_[0].size()); }
    int height() const { return static_cast<int>(grid_.size()); }

    bool isWall(int gx, int gy) const;
    bool isWallAtPixel(float px, float py, float radius) const;

    sf::Vector2f startPosition() const { return startPos_; }
    sf::Vector2f exitPosition() const { return exitPos_; }
    bool isFloor(int gx, int gy) const;
    sf::Vector2f nearestFloorCenter(int gx, int gy) const;
    sf::Vector2i pixelToGrid(sf::Vector2f p) const;
    sf::Vector2f gridCenter(sf::Vector2i g) const;
    // Возвращает следующую клетку на кратчайшем пути от from -> to (BFS).
    // Если пути нет, возвращает from.
    sf::Vector2i nextStepBfs(sf::Vector2i from, sf::Vector2i to) const;
    bool hasPathStartToExit() const;

    // Движение круга с раздельной проверкой по X и Y (скольжение вдоль стен в углах)
    void moveCircle(sf::Vector2f& pos, sf::Vector2f delta, float radius) const;

    void draw(sf::RenderTarget& target) const;
    void setFloorTexture(const sf::Texture* tex) { floorTex_ = tex; }
    void setWallTexture(const sf::Texture* tex) { wallTex_ = tex; }
    void setTileSize(unsigned size);
    unsigned tileSize() const { return tileSize_; }

private:
    bool isReachable(int sx, int sy, int ex, int ey) const;

    std::vector<std::string> grid_;
    sf::Vector2f startPos_{};
    sf::Vector2f exitPos_{};
    const sf::Texture* floorTex_{nullptr};
    const sf::Texture* wallTex_{nullptr};
    unsigned tileSize_{TILE_SIZE};
};
