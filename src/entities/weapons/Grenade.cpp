#include "Grenade.h"
#include "ThrownGrenade.h"
#include "../Entity.h"
#include "../../world/EntityManager.h"
#include <memory>

Grenade::Grenade(float dmg, float rate, float thrwSpeed,
    float fuse, float blastRad,
    int magSize, int reserve, float reload, AmmoType ammo)
    : Weapon("Grenade", dmg, rate, magSize, reserve, reload, /*spread=*/0.f, ammo)
    , throwSpeed(thrwSpeed)
    , fuseTime(fuse)
    , blastRadius(blastRad)
{
}

void Grenade::fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
    EntityManager& entities) {
    if (!canFire()) return;

    entities.add(std::make_unique<ThrownGrenade>(
        origin, direction, throwSpeed, fuseTime, blastRadius, damage, entities, &owner));

    consumeShot();
}