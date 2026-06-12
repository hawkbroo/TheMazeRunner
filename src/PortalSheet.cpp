#include "PortalSheet.hpp"

bool PortalSheet::loadFromFile(const std::string& path) {
    if (!texture.loadFromFile(path))
        return false;
    loaded = true;
    cellW = texture.getSize().x / COLS;
    cellH = texture.getSize().y / ROWS;
    texture.setSmooth(true);
    return true;
}

sf::IntRect PortalSheet::frameRect(int frame) const {
    const int f = frame % FRAME_COUNT;
    const int col = f % COLS;
    const int row = f / COLS;
    return sf::IntRect(col * static_cast<int>(cellW), row * static_cast<int>(cellH),
                       static_cast<int>(cellW), static_cast<int>(cellH));
}
