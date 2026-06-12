#pragma once

#include <SFML/Graphics.hpp>

#include "LevelData.hpp"
#include "Maze.hpp"
#include "MonsterPinkSheet.hpp"

enum class MonsterAnimState { Run, Transform, Evil, Kill, KillDone };

class Monster {
public:
    void spawn(const MonsterSpawn& cfg, float baseSpeed, const Maze& maze);
    void setPinkSheet(const MonsterPinkSheet* sheet) { pinkSheet_ = sheet; }

    void update(float dt, const Maze& maze, sf::Vector2f playerPos, float playerSpeed);
    void updateKill(float dt);

    void startKill();
    bool isKilling() const { return animState_ == MonsterAnimState::Kill; }
    bool killFinished() const { return animState_ == MonsterAnimState::KillDone; }

    void draw(sf::RenderTarget& target) const;

    sf::Vector2f position() const { return pos_; }
    float radius() const { return radius_; }

private:
    void updatePatrol(float dt, const Maze& maze);
    void updateChase(float dt, const Maze& maze, sf::Vector2f playerPos, float playerSpeed);
    void onChaseStarted();
    void onChaseCalmed();
    void advanceAnim(float dt, bool moving);
    void drawSprite(sf::RenderTarget& target) const;
    int mouthColFromDist(float dist) const;

    sf::Vector2f pos_{};
    float radius_{12.f};
    float speed_{60.f};
    float speedMultiplier_{1.f};
    float playerDist_{9999.f};
    MonsterAI ai_{MonsterAI::Patrol};
    int patrolDirX_{1};
    int patrolDirY_{0};
    bool lockPatrolAxis_{false};
    bool chasing_{false};
    bool evil_{false};
    bool facingLeft_{false};

    const MonsterPinkSheet* pinkSheet_{nullptr};

    MonsterAnimState animState_{MonsterAnimState::Run};
    int animFrame_{0};
    float animTimer_{0.f};
    bool lastMoving_{false};
};
