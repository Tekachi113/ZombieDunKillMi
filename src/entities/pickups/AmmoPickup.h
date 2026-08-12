#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>

// A world pickup that refills reserve ammo for every non-melee weapon
// the player is carrying when walked over. Spawned periodically and
// randomly across the map by AmmoSpawner (see AmmoSpawner.h).
class AmmoPickup : public Entity {
public:
    explicit AmmoPickup(sf::Vector2f pos, int refillAmount = 30);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

    int getRefillAmount() const { return refillAmount; }

    // Called by CollisionSystem once the player walks over this pickup.
    // Marks it dead so EntityManager::removeDead() clears it and it
    // can't be collected twice.
    void collect() { alive = false; }

private:
    int refillAmount;
    float bobTimer = 0.f; // small up/down bob, purely visual polish

    static constexpr float RADIUS = 10.f;
};
