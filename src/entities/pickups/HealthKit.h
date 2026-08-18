#pragma once

#include <SFML/Graphics.hpp>
#include "../Entity.h"
#include <optional>

class Player;

class HealthKit : public Entity
{
public:
    HealthKit(sf::Vector2f pos, Player* player);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

private:
    sf::RectangleShape shape; // fallback if texture is missing

    // assets/textures/pickups/hp.png -- placeholder art, swap the file
    // for real art later, no code changes needed.
    sf::Texture texture;
    std::optional<sf::Sprite> sprite;
    bool hasTexture = false;

    Player* playerRef;
};
