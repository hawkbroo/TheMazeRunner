#pragma once

#include <SFML/Graphics.hpp>

#include "Constants.hpp"
#include "Maze.hpp"
#include "PlayerSheet.hpp"

enum class PlayerAnimState { Idle, Run, Dying, Dead };

class Player {
public:
    void reset(sf::Vector2f startPos);
    void setSheet(const PlayerSheet* sheet) { sheet_ = sheet; }
    void setTexture(const sf::Texture* tex) { fallbackTex_ = tex; }
    void handleInput();
    void update(float dt, const Maze& maze);
    void updateDeath(float dt);

    void startDeath();
    bool isDying() const { return animState_ == PlayerAnimState::Dying; }
    bool deathFinished() const { return animState_ == PlayerAnimState::Dead; }

    sf::Vector2f position() const { return pos_; }
    float radius() const { return radius_; }
    float speed() const { return speed_; }

    void draw(sf::RenderTarget& target) const;

private:
    void advanceAnim(float dt);
    void drawSprite(sf::RenderTarget& target) const;

    sf::Vector2f pos_{};
    sf::Vector2f moveDir_{};
    sf::Vector2f lastFacing_{0.f, 1.f};
    float radius_{11.f};
    float speed_{PLAYER_SPEED};

    const PlayerSheet* sheet_{nullptr};
    const sf::Texture* fallbackTex_{nullptr};

    PlayerAnimState animState_{PlayerAnimState::Idle};
    int animFrame_{0};
    float animTimer_{0.f};
    int facingCol_{4};
};
