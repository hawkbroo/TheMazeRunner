#include "ExitPortal.hpp"

#include <algorithm>

void ExitPortal::update(float dt) {
    if (!sheet_ || !sheet_->loaded)
        return;
    timer_ += dt;
    while (timer_ >= PortalSheet::FRAME_TIME) {
        timer_ -= PortalSheet::FRAME_TIME;
        frame_ = (frame_ + 1) % PortalSheet::FRAME_COUNT;
    }
}

void ExitPortal::draw(sf::RenderTarget& target, float tileSize) const {
    const float displaySize = tileSize * 1.4f;

    if (sheet_ && sheet_->loaded) {
        sf::Sprite sprite(sheet_->texture);
        sprite.setTextureRect(sheet_->frameRect(frame_));
        const float cw = static_cast<float>(sheet_->cellW);
        const float ch = static_cast<float>(sheet_->cellH);
        const float scale = displaySize / std::max(cw, ch);
        sprite.setOrigin(cw * 0.5f, ch * 0.5f);
        sprite.setScale(scale, scale);
        sprite.setPosition(pos_);
        target.draw(sprite);
        return;
    }

    if (fallbackTex_) {
        sf::Sprite s(*fallbackTex_);
        s.setOrigin(fallbackTex_->getSize().x * 0.5f, fallbackTex_->getSize().y * 0.5f);
        s.setPosition(pos_);
        s.setScale(displaySize / fallbackTex_->getSize().x, displaySize / fallbackTex_->getSize().y);
        target.draw(s);
        return;
    }

    sf::CircleShape marker(tileSize * 0.35f);
    marker.setOrigin(marker.getRadius(), marker.getRadius());
    marker.setPosition(pos_);
    marker.setFillColor(sf::Color(80, 220, 120));
    target.draw(marker);
}
