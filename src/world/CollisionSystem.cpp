#include "CollisionSystem.h"
#include "EntityManager.h"
#include "TileMap.h"
#include "../entities/Entity.h"
#include "../entities/Player.h"
#include "../entities/zombies/Zombie.h"
#include "../entities/Projectile.h"
#include "../entities/world_objects/WorldObjects.h"
#include "../entities/pickups/AmmoPickup.h"
#include <cmath>



void CollisionSystem::resolve(EntityManager& entities, const TileMap& map, Player& player) {
    // 1. Tile collisions
    resolveTileCollisions(player, map);
    for (Entity* e : entities.getAll()) {
        if (e->isAlive()) resolveTileCollisions(*e, map);
    }

    // 2. Entity vs Entity / Player collisions
    resolveEntityCollisions(entities, player);
}

// ---- Tile Collision ------------------------------------

void CollisionSystem::resolveTileCollisions(Entity& entity, const TileMap& map) const {
   
    sf::FloatRect bounds = entity.getBounds();
    const int tileSize = map.getTileSize();

    
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
                bounds = entity.getBounds(); 
            }
        }
    }
}



void CollisionSystem::resolveEntityCollisions(EntityManager& entities, Player& player) {
    auto all = entities.getAll();

    // Helper to identify solid world obstacles (crates, barrels, solid scenery)
    auto isSolidObstacle = [](Entity* e) {
        if (dynamic_cast<BreakableBox*>(e) || dynamic_cast<ExplodingBarrel*>(e)) {
            return true;
        }
        if (auto* scen = dynamic_cast<SceneryObject*>(e)) {
            // Check if bounds width is non-zero (collidable)
            return scen->getBounds().size.x > 0.f;
        }
        return false;
    };

    // 1. Entity vs Entity interactions
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

            // Projectile hits Solid Obstacle (box, barrel, solid scenery like tree)
            if (auto* proj = dynamic_cast<Projectile*>(a)) {
                if (isSolidObstacle(b)) {
                    b->takeDamage(proj->getDamage());
                    proj->onHit();
                    continue;
                }
            }
            if (auto* proj = dynamic_cast<Projectile*>(b)) {
                if (isSolidObstacle(a)) {
                    a->takeDamage(proj->getDamage());
                    proj->onHit();
                    continue;
                }
            }

            // Zombie vs Solid Obstacle
            if (auto* zombie = dynamic_cast<Zombie*>(a)) {
                if (isSolidObstacle(b)) {
                    sf::Vector2f push = calcPushVector(zombie->getBounds(), b->getBounds());
                    zombie->setPosition(zombie->getPosition() + push);
                    continue;
                }
            }
            if (auto* zombie = dynamic_cast<Zombie*>(b)) {
                if (isSolidObstacle(a)) {
                    sf::Vector2f push = calcPushVector(zombie->getBounds(), a->getBounds());
                    zombie->setPosition(zombie->getPosition() + push);
                    continue;
                }
            }
        }
    }

    // 2. Player vs all other entities
    // 2. Player vs all other entities
    if (player.isAlive()) {
        for (Entity* e : all) {
            if (!e->isAlive()) continue;
            if (!aabbOverlap(player.getBounds(), e->getBounds())) continue;

            // Player picks up ammo (refills every non-melee weapon, then
            // the pickup marks itself dead so it can't be collected twice)
            if (auto* ammo = dynamic_cast<AmmoPickup*>(e)) {
                player.refillAllAmmo(ammo->getRefillAmount());
                ammo->collect();
                continue;
            }

            // Player vs Solid Obstacle (crates, barrels, trees)
            if (isSolidObstacle(e)) {
                sf::Vector2f push = calcPushVector(player.getBounds(), e->getBounds());
                player.setPosition(player.getPosition() + push);
            }
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
