#include "HealthKit.h"
#include "../Player.h"
#include <algorithm>

HealthKit::HealthKit(sf::Vector2f pos, Player* player)
    : Entity(pos), playerRef(player)
{
    shape.setSize({ 30.f, 30.f });
    shape.setPosition(pos);
    shape.setFillColor(sf::Color::Green);
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
    target.draw(shape);
}

sf::FloatRect HealthKit::getBounds() const
{
    return shape.getGlobalBounds();
}