#pragma once
#include <SFML/Graphics.hpp>
#include "../Entity.h"

class Player;

class ShieldPickup : public Entity {
public:
    ShieldPickup(sf::Vector2f pos, Player* player, float amount = 30.f);
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

private:
    sf::RectangleShape shape;
    Player* playerRef;
    float shieldAmount;
};
