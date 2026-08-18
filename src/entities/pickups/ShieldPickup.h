#pragma once
#include <SFML/Graphics.hpp>
#include "../Entity.h"
#include <optional>

class Player;

class ShieldPickup : public Entity {
public:
    ShieldPickup(sf::Vector2f pos, Player* player, float amount = 30.f);
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

private:
    sf::RectangleShape shape; // fallback if texture is missing

    // assets/textures/pickups/shield.png -- placeholder art (currently
    // a potion-bottle icon since the tileset has no dedicated shield
    // icon), swap the file for real art later, no code changes needed.
    sf::Texture texture;
    std::optional<sf::Sprite> sprite;
    bool hasTexture = false;

    Player* playerRef;
    float shieldAmount;
};
