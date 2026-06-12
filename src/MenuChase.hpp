#pragma once

#include <SFML/Graphics.hpp>

#include "MonsterPinkSheet.hpp"
#include "PlayerSheet.hpp"

// Декоративная погоня на главном меню.
class MenuChase {
public:
    void setSheets(const PlayerSheet* player, const MonsterPinkSheet* monster) {
        playerSheet_ = player;
        monsterSheet_ = monster;
    }

    void update(float dt, float laneWidth);
    void draw(sf::RenderTarget& target, float centerY, float laneWidth) const;

private:
    void drawPlayer(sf::RenderTarget& target, float x, float y, float displayH) const;
    void drawMonster(sf::RenderTarget& target, float x, float y, float displayH) const;

    const PlayerSheet* playerSheet_{nullptr};
    const MonsterPinkSheet* monsterSheet_{nullptr};

    float playerX_{-90.f};
    float monsterX_{-590.f};
    bool waitingForMonster_{false};
    float playerAnimTimer_{0.f};
    int playerRunFrame_{0};
    float monsterAnimTimer_{0.f};
    int monsterRunFrame_{0};

    static constexpr float SPEED = 240.f;
    static constexpr float MONSTER_OFFSET = 500.f;
    static constexpr float EDGE_PAD = 70.f;
    static constexpr float PLAYER_RUN_TIME = 0.11f;
    static constexpr float MONSTER_RUN_TIME = 0.09f;
};
