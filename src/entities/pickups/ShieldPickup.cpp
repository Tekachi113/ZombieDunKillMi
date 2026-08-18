#include "ShieldPickup.h"
#include "../Player.h"
#include <iostream>

ShieldPickup::ShieldPickup(sf::Vector2f pos, Player* player, float amount)
    : Entity(pos), playerRef(player), shieldAmount(amount)
{
    shape.setSize({ 30.f, 30.f });
    shape.setPosition(pos);
    shape.setFillColor(sf::Color::Cyan);

    if (texture.loadFromFile("assets/textures/pickups/shield.png")) {
        sprite.emplace(texture);
        sf::Vector2u size = texture.getSize();
        sprite->setOrigin({ size.x * 0.5f, size.y * 0.5f });
        sprite->setPosition(pos);
        sprite->setScale({ 2.2f, 2.2f }); // source art is tiny (32x22), scale up
        hasTexture = true;
    }
    else {
        std::cout << "[ShieldPickup] No texture at assets/textures/pickups/shield.png -- using placeholder shape\n";
    }
}

void ShieldPickup::update(float dt) {
    if (!isAlive() || playerRef == nullptr) return;
    if (getBounds().findIntersection(playerRef->getBounds())) {
        playerRef->addShield(shieldAmount);
        alive = false;
    }
}

void ShieldPickup::render(sf::RenderTarget& target) {
    if (hasTexture && sprite) {
        target.draw(*sprite);
        return;
    }
    target.draw(shape);
}

sf::FloatRect ShieldPickup::getBounds() const {
    if (hasTexture && sprite) {
        return sprite->getGlobalBounds();
    }
    return shape.getGlobalBounds();
}