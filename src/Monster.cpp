#include "Monster.hpp"

#include <algorithm>
#include <cmath>

#include "Constants.hpp"

namespace {
constexpr float RUN_FRAME_TIME = 0.09f;
constexpr float TRANSFORM_FRAME_TIME = 0.14f;
constexpr float KILL_FRAME_TIMES[] = {0.18f, 0.18f, 0.2f, 0.22f, 0.4f};

sf::Vector2f dirFromPatrol(int px, int py) {
    sf::Vector2f d(static_cast<float>(px), static_cast<float>(py));
    const float len = std::sqrt(d.x * d.x + d.y * d.y);
    if (len > 0.f) d /= len;
    return d;
}
} // namespace

void Monster::spawn(const MonsterSpawn& cfg, float baseSpeed, const Maze& maze) {
    pos_ = maze.nearestFloorCenter(cfg.gridX, cfg.gridY);
    patrolDirX_ = cfg.patrolDirX;
    patrolDirY_ = cfg.patrolDirY;
    lockPatrolAxis_ = cfg.lockPatrolAxis;
    ai_ = cfg.ai;
    chasing_ = false;
    evil_ = false;
    facingLeft_ = patrolDirX_ < 0;
    speedMultiplier_ = cfg.speedMultiplier;
    animState_ = MonsterAnimState::Run;
    animFrame_ = 0;
    animTimer_ = 0.f;
    lastMoving_ = false;
    playerDist_ = 9999.f;

    if (ai_ == MonsterAI::Patrol) {
        speed_ = baseSpeed * speedMultiplier_;
    } else {
        speed_ = 45.f * speedMultiplier_;
    }

}

void Monster::startKill() {
    if (animState_ == MonsterAnimState::Kill || animState_ == MonsterAnimState::KillDone)
        return;
    animState_ = MonsterAnimState::Kill;
    animFrame_ = 0;
    animTimer_ = 0.f;
}

void Monster::onChaseStarted() {
    if (ai_ != MonsterAI::Chase || !pinkSheet_ || !pinkSheet_->loaded || evil_ ||
        animState_ == MonsterAnimState::Transform || animState_ == MonsterAnimState::Kill ||
        animState_ == MonsterAnimState::KillDone)
        return;
    animState_ = MonsterAnimState::Transform;
    animFrame_ = 0;
    animTimer_ = 0.f;
}

void Monster::onChaseCalmed() {
    if (animState_ == MonsterAnimState::Kill || animState_ == MonsterAnimState::KillDone)
        return;
    if (!evil_ && animState_ != MonsterAnimState::Evil &&
        animState_ != MonsterAnimState::Transform)
        return;
    evil_ = false;
    animState_ = MonsterAnimState::Run;
    animFrame_ = 0;
    animTimer_ = 0.f;
}

int Monster::mouthColFromDist(float dist) const {
    if (dist > CHASE_START_DISTANCE) return 0;
    if (dist > 170.f) return 1;
    if (dist > 130.f) return 2;
    if (dist > 90.f) return 3;
    return 4;
}

void Monster::updatePatrol(float dt, const Maze& maze) {
    const sf::Vector2f wish = dirFromPatrol(patrolDirX_, patrolDirY_) * speed_ * dt;
    const sf::Vector2f before = pos_;
    maze.moveCircle(pos_, wish, radius_);

    if ((pos_ - before).x * (pos_ - before).x + (pos_ - before).y * (pos_ - before).y < 0.01f) {
        if (lockPatrolAxis_) {
            patrolDirX_ = -patrolDirX_;
            patrolDirY_ = -patrolDirY_;
            return;
        }
        const int options[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (const auto& opt : options) {
            sf::Vector2f testPos = pos_;
            const sf::Vector2f tryWish = dirFromPatrol(opt[0], opt[1]) * speed_ * dt;
            maze.moveCircle(testPos, tryWish, radius_);
            if ((testPos - pos_).x * (testPos - pos_).x + (testPos - pos_).y * (testPos - pos_).y > 0.01f) {
                patrolDirX_ = opt[0];
                patrolDirY_ = opt[1];
                pos_ = testPos;
                return;
            }
        }
        patrolDirX_ = -patrolDirX_;
        patrolDirY_ = -patrolDirY_;
    }
}

void Monster::updateChase(float dt, const Maze& maze, sf::Vector2f playerPos, float playerSpeed) {
    speed_ = playerSpeed * MONSTER_CHASE_FACTOR * speedMultiplier_;

    const float dx = playerPos.x - pos_.x;
    const float dy = playerPos.y - pos_.y;
    const float dist = std::sqrt(dx * dx + dy * dy);
    playerDist_ = dist;

    const bool wasChasing = chasing_;
    if (!chasing_ && dist < CHASE_START_DISTANCE) chasing_ = true;
    if (chasing_ && dist > CHASE_STOP_DISTANCE) chasing_ = false;

    if (!wasChasing && chasing_) onChaseStarted();

    if (!chasing_) {
        onChaseCalmed();
        updatePatrol(dt, maze);
        return;
    }

    if (animState_ == MonsterAnimState::Transform)
        return;

    const sf::Vector2i fromG = maze.pixelToGrid(pos_);
    const sf::Vector2i toG = maze.pixelToGrid(playerPos);
    const sf::Vector2i nextG = maze.nextStepBfs(fromG, toG);
    if (nextG == fromG) return;

    const sf::Vector2f target = maze.gridCenter(nextG);
    sf::Vector2f dir(target.x - pos_.x, target.y - pos_.y);
    const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.001f) dir /= len;

    maze.moveCircle(pos_, dir * speed_ * dt, radius_);
}

void Monster::update(float dt, const Maze& maze, sf::Vector2f playerPos, float playerSpeed) {
    if (animState_ == MonsterAnimState::Kill || animState_ == MonsterAnimState::KillDone) {
        advanceAnim(dt, false);
        return;
    }

    const sf::Vector2f before = pos_;
    if (ai_ == MonsterAI::Patrol) {
        updatePatrol(dt, maze);
        const float pdx = playerPos.x - pos_.x;
        const float pdy = playerPos.y - pos_.y;
        playerDist_ = std::sqrt(pdx * pdx + pdy * pdy);
    } else {
        updateChase(dt, maze, playerPos, playerSpeed);
    }

    const sf::Vector2f delta = pos_ - before;
    const float movedSq = delta.x * delta.x + delta.y * delta.y;
    lastMoving_ = movedSq > 0.25f;
    if (lastMoving_ && std::abs(delta.x) > 0.05f)
        facingLeft_ = delta.x < 0.f;
    else if (patrolDirX_ != 0)
        facingLeft_ = patrolDirX_ < 0;

    advanceAnim(dt, lastMoving_);
}

void Monster::updateKill(float dt) {
    if (animState_ != MonsterAnimState::Kill) return;
    advanceAnim(dt, false);
}

void Monster::advanceAnim(float dt, bool moving) {
    if (!pinkSheet_ || !pinkSheet_->loaded) return;

    animTimer_ += dt;

    if (animState_ == MonsterAnimState::Run) {
        if (!moving) {
            animFrame_ = 0;
            animTimer_ = 0.f;
            return;
        }
        if (animTimer_ >= RUN_FRAME_TIME) {
            animTimer_ -= RUN_FRAME_TIME;
            animFrame_ = (animFrame_ + 1) % MonsterPinkSheet::RUN_FRAMES;
        }
        return;
    }

    if (animState_ == MonsterAnimState::Evil) {
        if (moving && animTimer_ >= RUN_FRAME_TIME) {
            animTimer_ -= RUN_FRAME_TIME;
            animFrame_ = (animFrame_ + 1) % MonsterPinkSheet::RUN_FRAMES;
        }
        return;
    }

    if (animState_ == MonsterAnimState::Transform) {
        if (animTimer_ >= TRANSFORM_FRAME_TIME) {
            animTimer_ -= TRANSFORM_FRAME_TIME;
            ++animFrame_;
            if (animFrame_ >= MonsterPinkSheet::TRANSFORM_FRAMES) {
                animFrame_ = MonsterPinkSheet::TRANSFORM_FRAMES - 1;
                evil_ = true;
                animState_ = MonsterAnimState::Evil;
                animTimer_ = 0.f;
            }
        }
        return;
    }

    if (animState_ == MonsterAnimState::Kill) {
        const int frameIdx = std::min(animFrame_, MonsterPinkSheet::KILL_PLAY_FRAMES - 1);
        if (animTimer_ >= KILL_FRAME_TIMES[frameIdx]) {
            animTimer_ -= KILL_FRAME_TIMES[frameIdx];
            ++animFrame_;
            if (animFrame_ >= MonsterPinkSheet::KILL_PLAY_FRAMES) {
                animFrame_ = MonsterPinkSheet::KILL_PLAY_FRAMES - 1;
                animState_ = MonsterAnimState::KillDone;
            }
        }
    }
}

void Monster::draw(sf::RenderTarget& target) const {
    if (pinkSheet_ && pinkSheet_->loaded) {
        drawSprite(target);
        return;
    }

    sf::CircleShape body(radius_);
    body.setOrigin(radius_, radius_);
    body.setPosition(pos_);
    body.setFillColor((chasing_ || evil_) ? sf::Color(240, 90, 90) : sf::Color(220, 120, 60));
    body.setOutlineThickness(2.f);
    body.setOutlineColor(sf::Color(40, 20, 20));
    target.draw(body);
}

void Monster::drawSprite(sf::RenderTarget& target) const {
    int row = MonsterPinkSheet::ROW_RUN;
    int col = 0;

    if (animState_ == MonsterAnimState::Run) {
        row = MonsterPinkSheet::ROW_RUN;
        col = animFrame_;
    } else if (animState_ == MonsterAnimState::Transform) {
        row = MonsterPinkSheet::ROW_TRANSFORM;
        col = animFrame_;
    } else if (animState_ == MonsterAnimState::Evil) {
        row = MonsterPinkSheet::ROW_TRANSFORM;
        col = mouthColFromDist(playerDist_);
        if (col < 1) col = 1;
        if (lastMoving_ && animFrame_ % 2 == 1 && col < MonsterPinkSheet::TRANSFORM_FRAMES - 1)
            ++col;
    } else if (animState_ == MonsterAnimState::Kill || animState_ == MonsterAnimState::KillDone) {
        row = MonsterPinkSheet::ROW_KILL;
        col = std::min(animFrame_, MonsterPinkSheet::KILL_PLAY_FRAMES - 1);
    }

    sf::Sprite sprite(pinkSheet_->texture);
    sprite.setTextureRect(pinkSheet_->cellRect(col, row));

    const float cw = static_cast<float>(pinkSheet_->cellW);
    const float ch = static_cast<float>(pinkSheet_->cellH);
    const float displayH = radius_ * 4.0f;
    const float scale = displayH / ch;
    sprite.setOrigin(cw * 0.5f, ch * 0.55f);
    sprite.setScale(facingLeft_ ? -scale : scale, scale);
    sprite.setPosition(pos_);
    target.draw(sprite);
}
