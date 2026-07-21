#include "EntityManager.h"
#include "../entities/Entity.h"
#include <cmath>
#include <algorithm>

// ---- Add / Flush ---------------------------------------

void EntityManager::add(std::unique_ptr<Entity> entity) {
    // Queue for deferred insertion — safe during iteration
    toAdd.push_back(std::move(entity));
}

void EntityManager::flushPendingAdds() {
    for (auto& e : toAdd) {
        entities.push_back(std::move(e));
    }
    toAdd.clear();
}

// ---- Lifecycle -----------------------------------------

void EntityManager::update(float dt) {
    flushPendingAdds();
    for (auto& e : entities) {
        if (e->isAlive()) e->update(dt);
    }
}

void EntityManager::render(sf::RenderTarget& target) const {
    for (const auto& e : entities) {
        if (e->isAlive()) e->render(target);
    }
}

void EntityManager::removeDead() {
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
                       [](const std::unique_ptr<Entity>& e) { return !e->isAlive(); }),
        entities.end());
}

// ---- Queries -------------------------------------------

std::vector<Entity*> EntityManager::getAll() const {
    std::vector<Entity*> result;
    result.reserve(entities.size());
    for (const auto& e : entities) {
        result.push_back(e.get());
    }
    return result;
}

std::vector<Entity*> EntityManager::queryRadius(sf::Vector2f center, float radius) const {
    std::vector<Entity*> result;
    float r2 = radius * radius;
    for (const auto& e : entities) {
        if (!e->isAlive()) continue;
        sf::Vector2f diff = e->getPosition() - center;
        if (diff.x * diff.x + diff.y * diff.y <= r2) {
            result.push_back(e.get());
        }
    }
    return result;
}

int EntityManager::countAlive() const {
    int n = 0;
    for (const auto& e : entities) {
        if (e->isAlive()) ++n;
    }
    return n;
}
