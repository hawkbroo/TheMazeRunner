#pragma once

#include <SFML/Graphics.hpp>

#include "PlayerSheet.hpp"
#include "MonsterPinkSheet.hpp"
#include "PortalSheet.hpp"

struct ArtPack {
    PlayerSheet playerSheet;
    MonsterPinkSheet monsterPinkSheet;
    PortalSheet portalSheet;
    sf::Texture player;
    sf::Texture exitMarker;
    sf::Texture floorTile;
    bool floorTileLoaded{false};

    sf::Texture menuBackground;
    bool menuBackgroundLoaded{false};

    static constexpr int WALL_TEXTURE_COUNT = 5;
    sf::Texture wallTiles[WALL_TEXTURE_COUNT];
    bool wallTileLoaded[WALL_TEXTURE_COUNT]{};
};

ArtPack makeArt();
