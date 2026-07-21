#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>

// Forward declarations
class Entity;
class Player;

// =========================================================
//  EntityManager — owns all live entities except the Player
//
//  Usage:
//    manager.add(std::make_unique<SmallZombie>(...));
//    manager.update(dt);
//    manager.render(target);
//    manager.removeDead();   // call once per frame after update
// =========================================================
class EntityManager {
public:
    // Add a new entity (takes ownership)
    void add(std::unique_ptr<Entity> entity);

    // Lifecycle — call each frame in order
    void update(float dt);
    void render(sf::RenderTarget& target) const;

    // Remove entities that are !isAlive()  — call after update each frame
    void removeDead();

    // Query — returns raw pointers (non-owning) to all live entities
    std::vector<Entity*> getAll() const;

    // Query — radius search (circle), useful for AoE damage / aggro
    std::vector<Entity*> queryRadius(sf::Vector2f center, float radius) const;

    // Query — filter by type tag using dynamic_cast
    template<typename T>
    std::vector<T*> getAllOf() const {
        std::vector<T*> result;
        for (auto& e : entities) {
            if (auto* t = dynamic_cast<T*>(e.get())) {
                result.push_back(t);
            }
        }
        return result;
    }

    // Count — how many entities of each category
    int countAlive()  const;
    int countTotal()  const { return static_cast<int>(entities.size()); }

    // Wipe all entities (e.g. when leaving PlayState)
    void clear() { entities.clear(); }

private:
    std::vector<std::unique_ptr<Entity>> entities;

    // Entities queued for addition (added during update to avoid iterator invalidation)
    std::vector<std::unique_ptr<Entity>> toAdd;
    void flushPendingAdds();
};
