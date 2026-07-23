#pragma once


#include "Weapon.h"

// Ranged weapon — fires a single Projectile per shot. The starting weapon.
class Pistol : public Weapon {
public:
    Pistol(float damage, float fireRate, float projectileSpeed, float spread,
        int magazineSize, int reserveAmmo, float reloadTime, AmmoType ammoType);

    void fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
        EntityManager& entities) override;

private:
    float projectileSpeed;
};