#include "Maze.hpp"

#include <algorithm>
#include <queue>
#include <utility>

namespace {

void drawTiledTexture(sf::RenderTarget& target, sf::Sprite& sprite, const sf::Texture& tex,
                       int gridX, int gridY, float px, float py, float ts, float zoom) {
    const int texW = static_cast<int>(tex.getSize().x);
    const int texH = static_cast<int>(tex.getSize().y);
    const int srcCellW = std::max(1, static_cast<int>(texW / zoom));
    const int srcCellH = std::max(1, static_cast<int>(texH / zoom));
    const int srcX = (gridX * srcCellW) % texW;
    const int srcY = (gridY * srcCellH) % texH;
    const int cropW = std::min(srcCellW, texW - srcX);
    const int cropH = std::min(srcCellH, texH - srcY);
    sprite.setTexture(tex, true);
    sprite.setTextureRect(sf::IntRect(srcX, srcY, cropW, cropH));
    sprite.setScale(ts / static_cast<float>(cropW), ts / static_cast<float>(cropH));
    sprite.setPosition(px, py);
    target.draw(sprite);
}

} // namespace

void Maze::setTileSize(unsigned size) {
    tileSize_ = size > 0 ? size : TILE_SIZE;
}

bool Maze::loadFromLayout(const std::vector<std::string>& layout) {
    if (layout.empty()) return false;
    grid_ = layout;
    size_t w = 0;
    for (const auto& row : grid_) w = std::max(w, row.size());
    if (w == 0) return false;
    for (auto& row : grid_) {
        if (row.size() > w) return false;
        while (row.size() < w) row.push_back('#');
    }

    for (int y = 0; y < height(); ++y) {
        for (int x = 0; x < width(); ++x) {
            const char c = grid_[y][x];
            const sf::Vector2f center(
                x * tileSize_ + tileSize_ * 0.5f,
                y * tileSize_ + tileSize_ * 0.5f);
            if (c == 'S') startPos_ = center;
            if (c == 'E') exitPos_ = center;
            if (c == 'S' || c == 'E') grid_[y][x] = '.';
        }
    }
    return true;
}

bool Maze::isWall(int gx, int gy) const {
    if (gx < 0 || gy < 0 || gx >= width() || gy >= height()) return true;
    return grid_[gy][gx] == '#';
}

bool Maze::isReachable(int sx, int sy, int ex, int ey) const {
    if (!isFloor(sx, sy) || !isFloor(ex, ey)) return false;
    std::vector<std::vector<char>> vis(height(), std::vector<char>(width(), 0));
    std::queue<std::pair<int, int>> q;
    q.push({sx, sy});
    vis[sy][sx] = 1;

    const int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        if (x == ex && y == ey) return true;
        for (auto& d : dirs) {
            const int nx = x + d[0];
            const int ny = y + d[1];
            if (nx < 0 || ny < 0 || nx >= width() || ny >= height()) continue;
            if (vis[ny][nx]) continue;
            if (!isFloor(nx, ny)) continue;
            vis[ny][nx] = 1;
            q.push({nx, ny});
        }
    }
    return false;
}

sf::Vector2i Maze::pixelToGrid(sf::Vector2f p) const {
    return sf::Vector2i(static_cast<int>(p.x / tileSize_), static_cast<int>(p.y / tileSize_));
}

sf::Vector2f Maze::gridCenter(sf::Vector2i g) const {
    return sf::Vector2f(g.x * tileSize_ + tileSize_ * 0.5f, g.y * tileSize_ + tileSize_ * 0.5f);
}

sf::Vector2i Maze::nextStepBfs(sf::Vector2i from, sf::Vector2i to) const {
    if (from == to) return from;
    if (!isFloor(from.x, from.y) || !isFloor(to.x, to.y)) return from;

    const int w = width();
    const int h = height();
    std::vector<int> prev(w * h, -1);
    std::queue<sf::Vector2i> q;

    auto idx = [w](int x, int y) { return y * w + x; };
    const int start = idx(from.x, from.y);
    const int goal = idx(to.x, to.y);
    prev[start] = start;
    q.push(from);

    const sf::Vector2i dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
    while (!q.empty()) {
        const sf::Vector2i cur = q.front();
        q.pop();
        const int ci = idx(cur.x, cur.y);
        if (ci == goal) break;

        for (auto d : dirs) {
            const int nx = cur.x + d.x;
            const int ny = cur.y + d.y;
            if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
            if (!isFloor(nx, ny)) continue;
            const int ni = idx(nx, ny);
            if (prev[ni] != -1) continue;
            prev[ni] = ci;
            q.push({nx, ny});
        }
    }

    if (prev[goal] == -1) return from; // пути нет

    // Восстановление: идём назад от goal, пока предок не станет start
    int cur = goal;
    int parent = prev[cur];
    while (parent != start && parent != cur) {
        cur = parent;
        parent = prev[cur];
    }

    return sf::Vector2i(cur % w, cur / w);
}

bool Maze::hasPathStartToExit() const {
    const int sx = static_cast<int>(startPos_.x / tileSize_);
    const int sy = static_cast<int>(startPos_.y / tileSize_);
    const int ex = static_cast<int>(exitPos_.x / tileSize_);
    const int ey = static_cast<int>(exitPos_.y / tileSize_);
    return isReachable(sx, sy, ex, ey);
}

void Maze::moveCircle(sf::Vector2f& pos, sf::Vector2f delta, float radius) const {
    const float cr = radius * COLLISION_RADIUS_SCALE;

    const float newX = pos.x + delta.x;
    if (!isWallAtPixel(newX, pos.y, cr)) pos.x = newX;

    const float newY = pos.y + delta.y;
    if (!isWallAtPixel(pos.x, newY, cr)) pos.y = newY;
}

bool Maze::isFloor(int gx, int gy) const {
    if (gx < 0 || gy < 0 || gx >= width() || gy >= height()) return false;
    return grid_[gy][gx] != '#';
}

sf::Vector2f Maze::nearestFloorCenter(int gx, int gy) const {
    // Поиск ближайшей клетки-прохода "волной" (BFS по манхэттенскому расстоянию).
    // Нужно на случай некорректного спавна монстров: мы аккуратно переносим их в проход.
    const int maxR = std::max(width(), height());
    for (int r = 0; r <= maxR; ++r) {
        for (int dy = -r; dy <= r; ++dy) {
            const int dx = r - std::abs(dy);
            const int cand[2][2] = { {gx + dx, gy + dy}, {gx - dx, gy + dy} };
            for (int i = 0; i < 2; ++i) {
                const int x = cand[i][0];
                const int y = cand[i][1];
                if (isFloor(x, y)) {
                    return sf::Vector2f(x * tileSize_ + tileSize_ * 0.5f,
                                       y * tileSize_ + tileSize_ * 0.5f);
                }
            }
        }
    }
    return sf::Vector2f(gx * tileSize_ + tileSize_ * 0.5f,
                       gy * tileSize_ + tileSize_ * 0.5f);
}

bool Maze::isWallAtPixel(float px, float py, float radius) const {
    const int minGX = static_cast<int>((px - radius) / tileSize_);
    const int maxGX = static_cast<int>((px + radius) / tileSize_);
    const int minGY = static_cast<int>((py - radius) / tileSize_);
    const int maxGY = static_cast<int>((py + radius) / tileSize_);

    for (int gy = minGY; gy <= maxGY; ++gy)
        for (int gx = minGX; gx <= maxGX; ++gx)
            if (isWall(gx, gy)) {
                const float cellLeft = gx * static_cast<float>(tileSize_);
                const float cellTop = gy * static_cast<float>(tileSize_);
                const float cellRight = cellLeft + tileSize_;
                const float cellBottom = cellTop + tileSize_;

                const float closestX = std::max(cellLeft, std::min(px, cellRight));
                const float closestY = std::max(cellTop, std::min(py, cellBottom));
                const float dx = px - closestX;
                const float dy = py - closestY;
                if (dx * dx + dy * dy < radius * radius) return true;
            }
    return false;
}

void Maze::draw(sf::RenderTarget& target) const {
    const float ts = static_cast<float>(tileSize_);
    sf::RectangleShape tile(sf::Vector2f(ts, ts));

    sf::Sprite tileSprite;

    for (int y = 0; y < height(); ++y) {
        for (int x = 0; x < width(); ++x) {
            const float px = x * ts;
            const float py = y * ts;
            if (grid_[y][x] == '#') {
                if (wallTex_) {
                    drawTiledTexture(target, tileSprite, *wallTex_, x, y, px, py, ts,
                                     WALL_TEXTURE_ZOOM);
                } else {
                    tile.setPosition(px, py);
                    tile.setFillColor(sf::Color(32, 34, 46));
                    tile.setOutlineThickness(0.f);
                    target.draw(tile);
                }
            } else if (floorTex_) {
                drawTiledTexture(target, tileSprite, *floorTex_, x, y, px, py, ts,
                                 FLOOR_TEXTURE_ZOOM);
            } else {
                tile.setPosition(px, py);
                tile.setFillColor(sf::Color(52, 58, 78));
                tile.setOutlineThickness(-1.f);
                tile.setOutlineColor(sf::Color(64, 70, 92));
                target.draw(tile);
            }
        }
    }

}
