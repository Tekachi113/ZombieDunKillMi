#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>

class EntityManager;

// The physical grenade flying through the world after Grenade::fire().
// Travels in a straight line (losing speed for a lobbed feel), then
// detonates when its fuse runs out — dealing AoE damage to everything
// in blastRadius, itself included in the entity list so CollisionSystem
// simply lets it live out its lifetime.
class ThrownGrenade : public Entity {
public:
    ThrownGrenade(sf::Vector2f pos, sf::Vector2f direction, float throwSpeed,
        float fuseTime, float blastRadius, float blastDamage,
        EntityManager& entities, Entity* owner = nullptr);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

    // Detonates immediately (e.g. on hitting a wall in the future).
    // Safe to call more than once — only the first call has effect.
    void explode();

private:
    // Uses Entity::velocity / Entity::position directly — no shadow members.
    float fuseTimer;
    float blastRadius;
    float blastDamage;
    EntityManager& entities;
    Entity* owner;
    bool exploded = false;

    static constexpr float RADIUS = 6.f;
};