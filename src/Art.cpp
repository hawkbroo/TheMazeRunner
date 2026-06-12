#include "Art.hpp"

#include <algorithm>
#include <cmath>
#include <string>

namespace {

// Процедурная текстура круга (запасной спрайт игрока).
sf::Texture makeSoftCircle(unsigned size, sf::Color base, sf::Color rim, sf::Color shine) {
    sf::Image img;
    img.create(size, size, sf::Color::Transparent);

    const float cx = (size - 1) * 0.5f;
    const float cy = (size - 1) * 0.5f;
    const float r = (size - 2) * 0.5f;

    for (unsigned y = 0; y < size; ++y) {
        for (unsigned x = 0; x < size; ++x) {
            const float dx = (static_cast<float>(x) - cx);
            const float dy = (static_cast<float>(y) - cy);
            const float dist = std::sqrt(dx * dx + dy * dy);
            const float t = dist / r;

            if (t > 1.f) continue;

            float alpha = 1.f;
            const float edge = 0.92f;
            if (t > edge) alpha = std::clamp(1.f - (t - edge) / (1.f - edge), 0.f, 1.f);

            const float light = std::clamp(0.8f + (-dx - dy) / (2.2f * r), 0.55f, 1.05f);

            sf::Color c = base;
            c.r = static_cast<sf::Uint8>(std::clamp(c.r * light, 0.f, 255.f));
            c.g = static_cast<sf::Uint8>(std::clamp(c.g * light, 0.f, 255.f));
            c.b = static_cast<sf::Uint8>(std::clamp(c.b * light, 0.f, 255.f));

            if (t > 0.78f) {
                const float k = std::clamp((t - 0.78f) / (1.f - 0.78f), 0.f, 1.f);
                c.r = static_cast<sf::Uint8>(c.r * (1.f - k) + rim.r * k);
                c.g = static_cast<sf::Uint8>(c.g * (1.f - k) + rim.g * k);
                c.b = static_cast<sf::Uint8>(c.b * (1.f - k) + rim.b * k);
            }

            const float sx = (static_cast<float>(x) - (cx - r * 0.25f));
            const float sy = (static_cast<float>(y) - (cy - r * 0.25f));
            const float sdist = std::sqrt(sx * sx + sy * sy);
            if (sdist < r * 0.35f) {
                const float k = std::clamp(1.f - (sdist / (r * 0.35f)), 0.f, 1.f) * 0.55f;
                c.r = static_cast<sf::Uint8>(c.r * (1.f - k) + shine.r * k);
                c.g = static_cast<sf::Uint8>(c.g * (1.f - k) + shine.g * k);
                c.b = static_cast<sf::Uint8>(c.b * (1.f - k) + shine.b * k);
            }

            c.a = static_cast<sf::Uint8>(255.f * alpha);
            img.setPixel(x, y, c);
        }
    }

    sf::Texture tex;
    tex.loadFromImage(img);
    tex.setSmooth(true);
    return tex;
}

// Запасная текстура выхода, если нет portal_sheet.
sf::Texture makeExit(unsigned size) {
    sf::Image img;
    img.create(size, size, sf::Color::Transparent);

    const float cx = (size - 1) * 0.5f;
    const float cy = (size - 1) * 0.5f;
    const float r = (size - 2) * 0.5f;

    for (unsigned y = 0; y < size; ++y) {
        for (unsigned x = 0; x < size; ++x) {
            const float dx = (static_cast<float>(x) - cx);
            const float dy = (static_cast<float>(y) - cy);
            const float dist = std::sqrt(dx * dx + dy * dy);
            const float t = dist / r;
            if (t > 1.f) continue;

            const float ring = std::abs(t - 0.72f);
            float a = 0.f;
            sf::Color c(90, 230, 140);

            if (t < 0.55f) { a = 0.35f; c = sf::Color(70, 200, 120); }
            if (ring < 0.08f) { a = std::max(a, 1.f - ring / 0.08f); c = sf::Color(120, 255, 170); }
            if (t > 0.9f) a *= std::clamp(1.f - (t - 0.9f) / 0.1f, 0.f, 1.f);

            c.a = static_cast<sf::Uint8>(255.f * std::clamp(a, 0.f, 1.f));
            img.setPixel(x, y, c);
        }
    }

    sf::Texture tex;
    tex.loadFromImage(img);
    tex.setSmooth(true);
    return tex;
}

} // namespace

ArtPack makeArt() {
    ArtPack pack;
    const char* sheetPaths[] = {
        "assets/player_sheet.png",
        "../assets/player_sheet.png",
    };
    for (const char* path : sheetPaths) {
        if (pack.playerSheet.loadFromFile(path))
            break;
    }

    const char* monsterPaths[] = {
        "assets/monster_pink_sheet.png",
        "../assets/monster_pink_sheet.png",
    };
    for (const char* path : monsterPaths) {
        if (pack.monsterPinkSheet.loadFromFile(path))
            break;
    }

    const char* portalPaths[] = {
        "assets/portal_sheet.png",
        "../assets/portal_sheet.png",
    };
    for (const char* path : portalPaths) {
        if (pack.portalSheet.loadFromFile(path))
            break;
    }

    const char* floorPaths[] = {
        "assets/floor_tile.jpg",
        "../assets/floor_tile.jpg",
    };
    for (const char* path : floorPaths) {
        if (pack.floorTile.loadFromFile(path)) {
            pack.floorTile.setSmooth(false);
            pack.floorTileLoaded = true;
            break;
        }
    }

    const char* menuBgPaths[] = {
        "assets/menu_bg.png",
        "../assets/menu_bg.png",
    };
    for (const char* path : menuBgPaths) {
        if (pack.menuBackground.loadFromFile(path)) {
            pack.menuBackground.setSmooth(true);
            pack.menuBackgroundLoaded = true;
            break;
        }
    }

    for (int i = 0; i < ArtPack::WALL_TEXTURE_COUNT; ++i) {
        const std::string file = "assets/wall" + std::to_string(i + 1) + ".jpg";
        const std::string alt = "../" + file;
        if (pack.wallTiles[i].loadFromFile(file) || pack.wallTiles[i].loadFromFile(alt)) {
            pack.wallTiles[i].setSmooth(false);
            pack.wallTileLoaded[i] = true;
        }
    }

    pack.player = makeSoftCircle(64, sf::Color(90, 170, 255), sf::Color(20, 45, 85), sf::Color(235, 245, 255));
    pack.exitMarker = makeExit(64);
    return pack;
}

