#include "Player.hpp"

#include <algorithm>
#include <cmath>

#include "Constants.hpp"

namespace {
constexpr float RUN_FRAME_TIME = 0.11f;
constexpr float DEATH_FRAME_TIMES[] = {0.22f, 0.2f, 0.2f, 0.35f};

int directionToColumn(sf::Vector2f d) {
    if (std::abs(d.x) < 0.35f && std::abs(d.y) < 0.35f)
        return -1;

    if (std::abs(d.x) < 0.45f)
        return d.y > 0.f ? PlayerSheet::COL_FRONT : PlayerSheet::COL_BACK;
    if (std::abs(d.y) < 0.45f)
        return d.x > 0.f ? PlayerSheet::COL_RIGHT : PlayerSheet::COL_LEFT;
    if (d.x > 0.f && d.y > 0.f) return PlayerSheet::COL_FRONT_RIGHT;
    if (d.x < 0.f && d.y > 0.f) return PlayerSheet::COL_FRONT_LEFT;
    if (d.x < 0.f && d.y < 0.f) return PlayerSheet::COL_BACK_LEFT;
    return PlayerSheet::COL_BACK_RIGHT;
}
} // namespace

void Player::reset(sf::Vector2f startPos) {
    pos_ = startPos;
    moveDir_ = {0.f, 0.f};
    lastFacing_ = {0.f, 1.f};
    animState_ = PlayerAnimState::Idle;
    animFrame_ = 0;
    animTimer_ = 0.f;
    facingCol_ = PlayerSheet::COL_FRONT;
}

void Player::startDeath() {
    if (animState_ == PlayerAnimState::Dying || animState_ == PlayerAnimState::Dead)
        return;
    animState_ = PlayerAnimState::Dying;
    animFrame_ = 0;
    animTimer_ = 0.f;
}

void Player::handleInput() {
    if (animState_ == PlayerAnimState::Dying || animState_ == PlayerAnimState::Dead)
        return;

    moveDir_ = {0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        moveDir_.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        moveDir_.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        moveDir_.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        moveDir_.x += 1.f;

    if (moveDir_.x != 0.f || moveDir_.y != 0.f) {
        const float len = std::sqrt(moveDir_.x * moveDir_.x + moveDir_.y * moveDir_.y);
        moveDir_ /= len;
        lastFacing_ = moveDir_;
    }
}

void Player::update(float dt, const Maze& maze) {
    if (animState_ == PlayerAnimState::Dying || animState_ == PlayerAnimState::Dead)
        return;

    const bool moving = moveDir_.x != 0.f || moveDir_.y != 0.f;
    animState_ = moving ? PlayerAnimState::Run : PlayerAnimState::Idle;

    const int col = directionToColumn(lastFacing_);
    if (col >= 0)
        facingCol_ = col;

    maze.moveCircle(pos_, moveDir_ * speed_ * dt, radius_);
    advanceAnim(dt);
}

void Player::updateDeath(float dt) {
    if (animState_ != PlayerAnimState::Dying)
        return;
    advanceAnim(dt);
}

void Player::advanceAnim(float dt) {
    if (!sheet_ || !sheet_->loaded)
        return;

    animTimer_ += dt;

    if (animState_ == PlayerAnimState::Idle) {
        animFrame_ = 0;
        return;
    }

    if (animState_ == PlayerAnimState::Run) {
        const float frameTime = RUN_FRAME_TIME;
        if (animTimer_ >= frameTime) {
            animTimer_ -= frameTime;
            animFrame_ = 1 - animFrame_;
        }
        return;
    }

    if (animState_ == PlayerAnimState::Dying) {
        const int frameIdx = std::min(animFrame_, PlayerSheet::DEATH_FRAMES - 1);
        const float frameTime = DEATH_FRAME_TIMES[frameIdx];
        if (animTimer_ >= frameTime) {
            animTimer_ -= frameTime;
            ++animFrame_;
            if (animFrame_ >= PlayerSheet::DEATH_FRAMES) {
                animFrame_ = PlayerSheet::DEATH_FRAMES - 1;
                animState_ = PlayerAnimState::Dead;
            }
        }
    }
}

void Player::draw(sf::RenderTarget& target) const {
    if (sheet_ && sheet_->loaded) {
        drawSprite(target);
        return;
    }

    if (fallbackTex_) {
        sf::Sprite s(*fallbackTex_);
        const float size = radius_ * 2.f + 10.f;
        s.setOrigin(fallbackTex_->getSize().x * 0.5f, fallbackTex_->getSize().y * 0.5f);
        s.setPosition(pos_);
        s.setScale(size / fallbackTex_->getSize().x, size / fallbackTex_->getSize().y);
        target.draw(s);
        return;
    }

    sf::CircleShape body(radius_);
    body.setOrigin(radius_, radius_);
    body.setPosition(pos_);
    body.setFillColor(sf::Color(100, 180, 255));
    body.setOutlineThickness(2.f);
    body.setOutlineColor(sf::Color(20, 40, 80));
    target.draw(body);
}

void Player::drawSprite(sf::RenderTarget& target) const {
    int row = PlayerSheet::ROW_IDLE;
    if (animState_ == PlayerAnimState::Run)
        row = animFrame_ == 0 ? PlayerSheet::ROW_RUN_A : PlayerSheet::ROW_RUN_B;
    else if (animState_ == PlayerAnimState::Dying || animState_ == PlayerAnimState::Dead) {
        static constexpr int deathRows[] = {
            PlayerSheet::ROW_FALL,
            PlayerSheet::ROW_DEAD_BACK,
            PlayerSheet::ROW_DEAD_DOWN_A,
            PlayerSheet::ROW_DEAD_DOWN_B,
        };
        const int idx = std::min(animFrame_, PlayerSheet::DEATH_FRAMES - 1);
        row = deathRows[idx];
    }

    sf::Sprite sprite(sheet_->texture);
    sprite.setTextureRect(sheet_->cellRect(facingCol_, row));

    const float cw = static_cast<float>(sheet_->cellW);
    const float ch = static_cast<float>(sheet_->cellH);
    const float displayH = radius_ * 4.00f;
    const float scale = displayH / ch;
    sprite.setOrigin(cw * 0.5f, ch * 0.55f);
    sprite.setScale(scale, scale);
    sprite.setPosition(pos_);
    target.draw(sprite);
}
