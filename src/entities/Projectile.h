#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>

// =========================================================
//  Projectile — stub header (full implementation: Person B)
// =========================================================
class Projectile : public Entity {
public:
    Projectile(sf::Vector2f pos, sf::Vector2f direction,
               float speed, float damage, Entity* owner = nullptr);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

    float getDamage() const { return damage; }
    bool  hasPiercing() const { return piercing; }

    // Called by CollisionSystem when it hits something
    void onHit();

protected:
    sf::Vector2f direction;
    float        speed;
    float        damage;
    float        lifetime    = 3.f;
    bool         piercing    = false;
    Entity*      owner       = nullptr;

    // Simple circle collider radius
    static constexpr float RADIUS = 4.f;
};
