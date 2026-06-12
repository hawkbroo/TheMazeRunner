#pragma once

#include <SFML/Graphics.hpp>

#include "PortalSheet.hpp"

class ExitPortal {
public:
    void setPosition(sf::Vector2f pos) { pos_ = pos; }
    void setSheet(const PortalSheet* sheet) { sheet_ = sheet; }
    void setFallbackTexture(const sf::Texture* tex) { fallbackTex_ = tex; }

    void update(float dt);
    void draw(sf::RenderTarget& target, float tileSize) const;

private:
    sf::Vector2f pos_{};
    const PortalSheet* sheet_{nullptr};
    const sf::Texture* fallbackTex_{nullptr};
    int frame_{0};
    float timer_{0.f};
};
