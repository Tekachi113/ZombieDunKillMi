#include "Pistol.h"
#include "../Entity.h"
#include "../Projectile.h"
#include "../../world/EntityManager.h"
#include <memory>
#include <cmath>

Pistol::Pistol(float dmg, float rate, float projSpeed, float spr,
    int magSize, int reserve, float reload, AmmoType ammo)
    : Weapon("Pistol", dmg, rate, magSize, reserve, reload, spr, ammo)
    , projectileSpeed(projSpeed)
{
}

void Pistol::fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
    EntityManager& entities) {
    if (!canFire()) { autoReloadIfEmpty(); return; }

    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f aimDir = (len > 0.0001f) ? direction / len : sf::Vector2f{ 1.f, 0.f };
    sf::Vector2f shotDir = applySpread(aimDir, spread);

    entities.add(std::make_unique<Projectile>(origin, shotDir, projectileSpeed, damage, &owner));

    consumeShot();
    autoReloadIfEmpty();
}
