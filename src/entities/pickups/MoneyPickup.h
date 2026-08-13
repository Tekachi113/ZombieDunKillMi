#pragma once
#include <SFML/Graphics.hpp>
#include "../Entity.h"

class Player;

class MoneyPickup : public Entity {
public:
    MoneyPickup(sf::Vector2f pos, Player* player, int amount);
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

private:
    sf::RectangleShape shape;
    Player* playerRef;
    int moneyAmount;
};
