#include "MonsterPinkSheet.hpp"

// Делит текстуру на сетку COLS x ROWS.
bool MonsterPinkSheet::loadFromFile(const std::string& path) {
    if (!texture.loadFromFile(path))
        return false;
    loaded = true;
    cellW = texture.getSize().x / COLS;
    cellH = texture.getSize().y / ROWS;
    texture.setSmooth(true);
    return true;
}

// Прямоугольник кадра по индексам колонки и ряда.
sf::IntRect MonsterPinkSheet::cellRect(int col, int row) const {
    return sf::IntRect(col * static_cast<int>(cellW), row * static_cast<int>(cellH),
                       static_cast<int>(cellW), static_cast<int>(cellH));
}
