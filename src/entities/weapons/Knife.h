#pragma once

#include "Weapon.h"

// Melee weapon — no ammo, no projectile. On fire() it sweeps a short-range
// arc in front of the owner and damages every alive entity caught in it
// (zombies AND breakable world objects).
class Knife : public Weapon {
public:
    explicit Knife(float damage, float attackRate, float range, float swingArcDeg);

    void fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
        EntityManager& entities) override;

private:
    float range;       // how far the swing reaches
    float swingArcDeg; // total arc width, centered on `direction`
};
