#include "MenuChase.hpp"

#include <algorithm>

namespace {
constexpr float PLAYER_ORIGIN_Y = 0.55f;
constexpr float MONSTER_ORIGIN_Y = 0.70f;
constexpr float MONSTER_SCALE = 1.06f;

// Выравнивает монстра по «ногам» игрока на полосе меню.
float alignedMonsterY(float centerY, float displayH) {
    const float playerBottom = centerY + (1.f - PLAYER_ORIGIN_Y) * displayH;
    const float monsterH = displayH * MONSTER_SCALE;
    return playerBottom - (1.f - MONSTER_ORIGIN_Y) * monsterH;
}
} // namespace

// Цикл: бег -> поимка -> монстр уезжает -> снова бег.
void MenuChase::update(float dt, float laneWidth) {
    const float right = laneWidth + EDGE_PAD;

    if (waitingForMonster_) {
        monsterX_ += SPEED * dt;
        if (monsterX_ > right) {
            waitingForMonster_ = false;
            playerX_ = -EDGE_PAD;
            monsterX_ = -EDGE_PAD - MONSTER_OFFSET;
        }
    } else {
        playerX_ += SPEED * dt;
        monsterX_ += SPEED * dt;

        if (playerX_ > monsterX_ && playerX_ - monsterX_ < MONSTER_OFFSET)
            monsterX_ = playerX_ - MONSTER_OFFSET;

        if (playerX_ > right) {
            waitingForMonster_ = true;
            playerX_ = right + EDGE_PAD;
        }
    }

    if (playerSheet_ && playerSheet_->loaded) {
        playerAnimTimer_ += dt;
        if (playerAnimTimer_ >= PLAYER_RUN_TIME) {
            playerAnimTimer_ -= PLAYER_RUN_TIME;
            playerRunFrame_ = 1 - playerRunFrame_;
        }
    }

    if (monsterSheet_ && monsterSheet_->loaded) {
        monsterAnimTimer_ += dt;
        if (monsterAnimTimer_ >= MONSTER_RUN_TIME) {
            monsterAnimTimer_ -= MONSTER_RUN_TIME;
            monsterRunFrame_ = (monsterRunFrame_ + 1) % MonsterPinkSheet::RUN_FRAMES;
        }
    }
}

void MenuChase::drawPlayer(sf::RenderTarget& target, float x, float y, float displayH) const {
    const int row = playerRunFrame_ == 0 ? PlayerSheet::ROW_RUN_A : PlayerSheet::ROW_RUN_B;

    sf::Sprite sprite(playerSheet_->texture);
    sprite.setTextureRect(playerSheet_->cellRect(PlayerSheet::COL_RIGHT, row));

    const float cw = static_cast<float>(playerSheet_->cellW);
    const float ch = static_cast<float>(playerSheet_->cellH);
    const float scale = displayH / ch;
    sprite.setOrigin(cw * 0.5f, ch * PLAYER_ORIGIN_Y);
    sprite.setScale(scale, scale);
    sprite.setPosition(x, y);
    target.draw(sprite);
}

void MenuChase::drawMonster(sf::RenderTarget& target, float x, float y, float displayH) const {
    int col = MonsterPinkSheet::TRANSFORM_FRAMES - 1;
    if (monsterRunFrame_ % 2 == 1 && col > 1)
        --col;

    sf::Sprite sprite(monsterSheet_->texture);
    sprite.setTextureRect(monsterSheet_->cellRect(col, MonsterPinkSheet::ROW_TRANSFORM));

    const float cw = static_cast<float>(monsterSheet_->cellW);
    const float ch = static_cast<float>(monsterSheet_->cellH);
    const float scale = displayH / ch;
    sprite.setOrigin(cw * 0.5f, ch * MONSTER_ORIGIN_Y);
    sprite.setScale(scale, scale);
    sprite.setPosition(x, y);
    target.draw(sprite);
}

void MenuChase::draw(sf::RenderTarget& target, float centerY, float laneWidth) const {
    const float displayH = std::max(92.f, laneWidth * 0.088f);
    const float monsterH = displayH * MONSTER_SCALE;
    const float monsterY = alignedMonsterY(centerY, displayH);

    if (monsterSheet_ && monsterSheet_->loaded)
        drawMonster(target, monsterX_, monsterY, monsterH);
    if (!waitingForMonster_ && playerSheet_ && playerSheet_->loaded)
        drawPlayer(target, playerX_, centerY, displayH);
}
