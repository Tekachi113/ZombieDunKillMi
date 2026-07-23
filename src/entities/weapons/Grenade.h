#pragma once

#include "Weapon.h"

// Thrown weapon — fire() doesn't damage anything directly, it spawns a
// ThrownGrenade entity which handles its own flight + fuse + AoE explosion.
class Grenade : public Weapon {
public:
    Grenade(float damage, float throwRate, float throwSpeed,
        float fuseTime, float blastRadius,
        int magazineSize, int reserveAmmo, float reloadTime, AmmoType ammoType);

    void fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
        EntityManager& entities) override;

private:
    float throwSpeed;
    float fuseTime;
    float blastRadius;
};
