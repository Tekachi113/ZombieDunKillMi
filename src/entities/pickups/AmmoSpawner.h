#pragma once

#include <SFML/Graphics.hpp>

class EntityManager;
class TileMap;

// Periodically spawns an AmmoPickup at a random point inside the given
// bounds. Lives in entities/pickups/ rather than world/SpawnManager
// since it only spawns pickups (not zombies) -- keeps it self-contained
// and easy to tune independently, e.g. from a settings/difficulty menu.
class AmmoSpawner {
public:
    // intervalSeconds: time between spawn attempts (adjustable at
    // runtime via setInterval()).
    // maxAlive: caps how many un-collected pickups can exist on the map
    // at once, so it doesn't get flooded if the player ignores them.
    // refillAmount: reserve ammo granted per weapon on pickup.
    explicit AmmoSpawner(float intervalSeconds = 20.f, int maxAlive = 3, int refillAmount = 30);

    void  setInterval(float seconds) { interval = seconds; }
    float getInterval() const { return interval; }

    void setMaxAlive(int count) { maxAlive = count; }
    void setRefillAmount(int amount) { refillAmount = amount; }

    // spawnBounds: the map area to scatter pickups across.
    // tileMap (optional): if set, spawn points are re-rolled a few
    // times to try to land on a walkable tile instead of inside a wall.
    void update(float dt, EntityManager& entities, const sf::FloatRect& spawnBounds,
        const TileMap* tileMap = nullptr);

private:
    sf::Vector2f randomPointIn(const sf::FloatRect& bounds, const TileMap* tileMap) const;

    float interval;
    float timer = 0.f;
    int   maxAlive;
    int   refillAmount;
};
