#include "Shotgun.h"
#include "../Entity.h"
#include "../Projectile.h"
#include "../../world/EntityManager.h"
#include <memory>
#include <cmath>

Shotgun::Shotgun(float dmgPerPellet, float rate, float projSpeed,
    float spr, int pellets,
    int magSize, int reserve, float reload, AmmoType ammo)
    : Weapon("Shotgun", dmgPerPellet, rate, magSize, reserve, reload, spr, ammo)
    , projectileSpeed(projSpeed)
    , pelletsPerShot(pellets)
{
}

void Shotgun::fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
    EntityManager& entities) {
    if (!canFire()) { autoReloadIfEmpty(); return; }

    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f aimDir = (len > 0.0001f) ? direction / len : sf::Vector2f{ 1.f, 0.f };

    // One shell fired = every pellet spawns, each with its own random spread
    for (int i = 0; i < pelletsPerShot; ++i) {
        sf::Vector2f pelletDir = applySpread(aimDir, spread);
        entities.add(std::make_unique<Projectile>(origin, pelletDir, projectileSpeed, damage, &owner));
    }

    consumeShot(); // consumes 1 shell, not 1 per pellet
    autoReloadIfEmpty();
}