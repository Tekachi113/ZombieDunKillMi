#include "ShieldPickup.h"
#include "../Player.h"

ShieldPickup::ShieldPickup(sf::Vector2f pos, Player* player, float amount)
    : Entity(pos), playerRef(player), shieldAmount(amount)
{
    shape.setSize({ 30.f, 30.f });
    shape.setPosition(pos);
    shape.setFillColor(sf::Color::Cyan);
}

void ShieldPickup::update(float dt) {
    if (!isAlive() || playerRef == nullptr) return;
    if (getBounds().findIntersection(playerRef->getBounds())) {
        playerRef->addShield(shieldAmount);
        alive = false;
    }
}

void ShieldPickup::render(sf::RenderTarget& target) { target.draw(shape); }
sf::FloatRect ShieldPickup::getBounds() const { return shape.getGlobalBounds(); }