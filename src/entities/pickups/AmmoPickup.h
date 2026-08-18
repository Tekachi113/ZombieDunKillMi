#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>
#include <optional>

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

    // assets/textures/pickups/ammo.png -- placeholder art (swap the
    // file whenever real art is ready, no code changes needed). Falls
    // back to the colored diamond shape if the file is missing.
    sf::Texture texture;
    std::optional<sf::Sprite> sprite;
    bool hasTexture = false;

    static constexpr float RADIUS = 10.f;
};
