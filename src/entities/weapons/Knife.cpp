#include "Knife.h"
#include "../Entity.h"
#include "../../world/EntityManager.h"
#include <cmath>
#include <algorithm>

Knife::Knife(float damage, float attackRate, float rng, float arcDeg)
    : Weapon("Knife", damage, attackRate, /*magazineSize=*/0, /*reserveAmmo=*/0,
        /*reloadTime=*/0.f, /*spread=*/0.f, AmmoType::None)
    , range(rng)
    , swingArcDeg(arcDeg)
{
}

void Knife::fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
    EntityManager& entities) {
    if (!canFire()) return;

    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f aimDir = (len > 0.0001f) ? direction / len : sf::Vector2f{ 1.f, 0.f };
    float halfArc = swingArcDeg * 0.5f;

    for (Entity* target : entities.queryRadius(origin, range)) {
        if (target == &owner || !target->isAlive()) continue;

        sf::Vector2f toTarget = target->getPosition() - origin;
        float dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
        if (dist < 0.001f) {
            target->takeDamage(damage); // right on top of us — always hits
            continue;
        }

        sf::Vector2f toTargetDir = toTarget / dist;
        float dot = std::clamp(aimDir.x * toTargetDir.x + aimDir.y * toTargetDir.y, -1.f, 1.f);
        float angleDeg = std::acos(dot) * 180.f / 3.14159265f;

        if (angleDeg <= halfArc) {
            target->takeDamage(damage);
        }
    }

    consumeShot();
}