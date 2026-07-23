#pragma once

#include "Weapon.h"

// Ranged weapon — fires several pellets (each its own Projectile) in a
// spread cone per single trigger pull. Short effective range in practice
// because the pellets themselves fan out and lose accuracy with distance.
class Shotgun : public Weapon {
public:
    Shotgun(float damagePerPellet, float fireRate, float projectileSpeed,
        float spread, int pelletsPerShot,
        int magazineSize, int reserveAmmo, float reloadTime, AmmoType ammoType);

    void fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
        EntityManager& entities) override;

private:
    float projectileSpeed;
    int   pelletsPerShot;
};
