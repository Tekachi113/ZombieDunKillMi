#include "CollisionSystem.h"
#include "EntityManager.h"
#include "TileMap.h"
#include "../entities/Entity.h"
#include "../entities/Player.h"
#include "../entities/zombies/Zombie.h"
#include "../entities/Projectile.h"
#include "../entities/world_objects/WorldObjects.h"
#include <cmath>

// ---- Public entry point --------------------------------

void CollisionSystem::resolve(EntityManager& entities, const TileMap& map) {
    // 1. Push all entities out of solid tiles
    for (Entity* e : entities.getAll()) {
        if (e->isAlive()) resolveTileCollisions(*e, map);
    }

    // 2. Entity vs Entity interactions
    resolveEntityCollisions(entities);
}

// ---- Tile Collision ------------------------------------

void CollisionSystem::resolveTileCollisions(Entity& entity, const TileMap& map) const {
    // Sample the four corners of the entity's bounding box
    sf::FloatRect bounds = entity.getBounds();
    const int tileSize = map.getTileSize();

    // Expand check range by one tile
    int colMin = static_cast<int>(bounds.position.x) / tileSize - 1;
    int colMax = static_cast<int>(bounds.position.x + bounds.size.x) / tileSize + 1;
    int rowMin = static_cast<int>(bounds.position.y) / tileSize - 1;
    int rowMax = static_cast<int>(bounds.position.y + bounds.size.y) / tileSize + 1;

    for (int row = rowMin; row <= rowMax; ++row) {
        for (int col = colMin; col <= colMax; ++col) {
            sf::Vector2f tileCenter {
                static_cast<float>(col * tileSize) + tileSize * 0.5f,
                static_cast<float>(row * tileSize) + tileSize * 0.5f
            };
            if (map.isWalkable(tileCenter)) continue;

            sf::FloatRect tileBounds {
                { static_cast<float>(col * tileSize), static_cast<float>(row * tileSize) },
                { static_cast<float>(tileSize), static_cast<float>(tileSize) }
            };

            if (aabbOverlap(bounds, tileBounds)) {
                sf::Vector2f push = calcPushVector(bounds, tileBounds);
                sf::Vector2f pos  = entity.getPosition();
                entity.setPosition(pos + push);
                bounds = entity.getBounds(); // recalculate after push
            }
        }
    }
}

// ---- Entity vs Entity ----------------------------------

void CollisionSystem::resolveEntityCollisions(EntityManager& entities) {
    auto all = entities.getAll();

    for (std::size_t i = 0; i < all.size(); ++i) {
        for (std::size_t j = i + 1; j < all.size(); ++j) {
            Entity* a = all[i];
            Entity* b = all[j];
            if (!a->isAlive() || !b->isAlive()) continue;
            if (!aabbOverlap(a->getBounds(), b->getBounds())) continue;

            // Projectile hits Zombie
            if (auto* proj = dynamic_cast<Projectile*>(a)) {
                if (auto* zombie = dynamic_cast<Zombie*>(b)) {
                    zombie->takeDamage(proj->getDamage());
                    proj->onHit();
                    continue;
                }
            }
            if (auto* proj = dynamic_cast<Projectile*>(b)) {
                if (auto* zombie = dynamic_cast<Zombie*>(a)) {
                    zombie->takeDamage(proj->getDamage());
                    proj->onHit();
                    continue;
                }
            }

            // Zombie vs breakable box — zombies don't interact with boxes
            // Player collision is handled in PlayState (Player is not in EntityManager)
        }
    }
}

// ---- Helpers -------------------------------------------

bool CollisionSystem::circleVsRect(sf::Vector2f center, float radius,
                                    const sf::FloatRect& rect) {
    float nearX = std::max(rect.position.x, std::min(center.x, rect.position.x + rect.size.x));
    float nearY = std::max(rect.position.y, std::min(center.y, rect.position.y + rect.size.y));
    float dx = center.x - nearX;
    float dy = center.y - nearY;
    return (dx * dx + dy * dy) < (radius * radius);
}

bool CollisionSystem::aabbOverlap(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.findIntersection(b).has_value();
}

sf::Vector2f CollisionSystem::calcPushVector(const sf::FloatRect& a,
                                              const sf::FloatRect& b) {
    float overlapX = std::min(a.position.x + a.size.x, b.position.x + b.size.x)
                   - std::max(a.position.x, b.position.x);
    float overlapY = std::min(a.position.y + a.size.y, b.position.y + b.size.y)
                   - std::max(a.position.y, b.position.y);

    if (overlapX < overlapY) {
        float sign = (a.position.x < b.position.x) ? -1.f : 1.f;
        return { sign * overlapX, 0.f };
    } else {
        float sign = (a.position.y < b.position.y) ? -1.f : 1.f;
        return { 0.f, sign * overlapY };
    }
}
