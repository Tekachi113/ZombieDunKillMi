#include "AmmoSpawner.h"
#include "AmmoPickup.h"
#include "../../world/EntityManager.h"
#include "../../world/TileMap.h"
#include <memory>
#include <cstdlib>
#include <algorithm>

AmmoSpawner::AmmoSpawner(float intervalSeconds, int maxAliveCount, int refillAmt)
    : interval(intervalSeconds)
    , maxAlive(maxAliveCount)
    , refillAmount(refillAmt)
{
}

sf::Vector2f AmmoSpawner::randomPointIn(const sf::FloatRect& bounds, const TileMap* tileMap) const {
    int w = std::max(1, static_cast<int>(bounds.size.x));
    int h = std::max(1, static_cast<int>(bounds.size.y));

    sf::Vector2f p{
        bounds.position.x + static_cast<float>(std::rand() % w),
        bounds.position.y + static_cast<float>(std::rand() % h)
    };

    if (!tileMap) return p;

    // Try a handful of times to land on a walkable tile; if none of the
    // attempts work out, just use the last one -- a slightly-off spawn
    // is harmless, CollisionSystem will nudge it off any wall anyway.
    int attempts = 0;
    while (!tileMap->isWalkable(p) && attempts < 10) {
        p = {
            bounds.position.x + static_cast<float>(std::rand() % w),
            bounds.position.y + static_cast<float>(std::rand() % h)
        };
        ++attempts;
    }
    return p;
}

void AmmoSpawner::update(float dt, EntityManager& entities, const sf::FloatRect& spawnBounds,
    const TileMap* tileMap) {
    timer += dt;
    if (timer < interval) return;
    timer = 0.f;

    int alive = static_cast<int>(entities.getAllOf<AmmoPickup>().size());
    if (alive >= maxAlive) return; // map already has enough uncollected ammo lying around

    sf::Vector2f pos = randomPointIn(spawnBounds, tileMap);
    entities.add(std::make_unique<AmmoPickup>(pos, refillAmount));
}