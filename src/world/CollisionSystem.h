#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// Forward declarations
class Entity;
class EntityManager;
class TileMap;

// =========================================================
//  CollisionSystem — resolves collisions each frame
// =========================================================
class CollisionSystem {
public:
    // Main entry point — call once per frame after entity updates
    void resolve(EntityManager& entities, const TileMap& map);

private:
    // --- Tile collision ---
    // Push entity out of any solid tiles it overlaps
    void resolveTileCollisions(Entity& entity, const TileMap& map) const;

    // --- Entity vs Entity ---
    // Damage/response logic for specific entity type pairs
    void resolveEntityCollisions(EntityManager& entities);

    // --- Primitive helpers ---
    // Circle vs AABB (tile) overlap test
    static bool circleVsRect(sf::Vector2f center, float radius,
                              const sf::FloatRect& rect);

    // AABB overlap test
    static bool aabbOverlap(const sf::FloatRect& a, const sf::FloatRect& b);

    // Returns a minimal translation vector to push `a` out of `b`
    static sf::Vector2f calcPushVector(const sf::FloatRect& a,
                                       const sf::FloatRect& b);
};
