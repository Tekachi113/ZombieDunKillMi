#include "AmmoPickup.h"
#include <cmath>

AmmoPickup::AmmoPickup(sf::Vector2f pos, int amount)
    : Entity(pos)
    , refillAmount(amount)
{
    health = maxHealth = 1.f; // not meant to be damaged, just walked over
}

void AmmoPickup::update(float dt) {
    bobTimer += dt;
}

void AmmoPickup::render(sf::RenderTarget& target) {
    // Placeholder: small yellow diamond with a gentle bob, so it reads
    // as "ammo" even before real pickup art exists.
    float bob = std::sin(bobTimer * 3.f) * 3.f;

    sf::RectangleShape shape({ RADIUS * 1.6f, RADIUS * 1.6f });
    shape.setOrigin({ RADIUS * 0.8f, RADIUS * 0.8f });
    shape.setPosition({ position.x, position.y + bob });
    shape.setRotation(sf::degrees(45.f));
    shape.setFillColor(sf::Color(230, 200, 60));
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(1.f);
    target.draw(shape);
}

sf::FloatRect AmmoPickup::getBounds() const {
    return sf::FloatRect{
        { position.x - RADIUS, position.y - RADIUS },
        { RADIUS * 2.f, RADIUS * 2.f }
    };
}