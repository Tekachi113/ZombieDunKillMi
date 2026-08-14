#pragma once

#include <SFML/Graphics.hpp>
#include "../Entity.h"

class Player;

class HealthKit : public Entity
{
public:
    HealthKit(sf::Vector2f pos, Player* player);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

private:
    sf::RectangleShape shape;
    Player* playerRef;
};
