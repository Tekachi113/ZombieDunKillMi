#include "HealthKit.h"
#include "../Player.h"
#include <algorithm>
#include <iostream>

HealthKit::HealthKit(sf::Vector2f pos, Player* player)
    : Entity(pos), playerRef(player)
{
    shape.setSize({ 30.f, 30.f });
    shape.setPosition(pos);
    shape.setFillColor(sf::Color::Green);

    if (texture.loadFromFile("assets/textures/pickups/hp.png")) {
        sprite.emplace(texture);
        sf::Vector2u size = texture.getSize();
        sprite->setOrigin({ size.x * 0.5f, size.y * 0.5f });
        sprite->setPosition(pos);
        sprite->setScale({ 2.2f, 2.2f }); // source art is tiny (32x22), scale up
        hasTexture = true;
    }
    else {
        std::cout << "[HealthKit] No texture at assets/textures/pickups/hp.png -- using placeholder shape\n";
    }
}

void HealthKit::update(float dt)
{
    if (!isAlive() || playerRef == nullptr)
        return;

    if (getBounds().findIntersection(playerRef->getBounds()))
    {
        if (playerRef->getHealth() >= playerRef->getMaxHealth())
            return;
        playerRef->addHealth(30.f);
        alive = false;
    }
}

void HealthKit::render(sf::RenderTarget& target)
{
    if (hasTexture && sprite) {
        target.draw(*sprite);
        return;
    }
    target.draw(shape);
}

sf::FloatRect HealthKit::getBounds() const
{
    if (hasTexture && sprite) {
        return sprite->getGlobalBounds();
    }
    return shape.getGlobalBounds();
}