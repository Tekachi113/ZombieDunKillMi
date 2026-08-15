#include "MoneyPickup.h"
#include "../Player.h"

MoneyPickup::MoneyPickup(sf::Vector2f pos, Player* player, int amount)
    : Entity(pos), playerRef(player), moneyAmount(amount)
{
    shape.setSize({ 24.f, 24.f });
    shape.setPosition(pos);
    shape.setFillColor(sf::Color::Yellow);
}

void MoneyPickup::update(float dt) {
    if (!isAlive() || playerRef == nullptr) return;
    if (getBounds().findIntersection(playerRef->getBounds())) {
        playerRef->addMoney(moneyAmount);
        alive = false;
    }
}

void MoneyPickup::render(sf::RenderTarget& target) { target.draw(shape); }
sf::FloatRect MoneyPickup::getBounds() const { return shape.getGlobalBounds(); }