#pragma once

#include <SFML/Graphics.hpp>

// Forward declarations
class ResourceManager;

// =========================================================
//  Entity — base class for every live object in the world
// =========================================================
class Entity {
public:
    explicit Entity(sf::Vector2f pos = {0.f, 0.f});
    virtual ~Entity() = default;

    // Core lifecycle — all subclasses must implement
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;

    // Collision shape (AABB)
    virtual sf::FloatRect getBounds() const = 0;

    // Health / damage
    virtual void takeDamage(float amount);
    virtual void onDeath() {}
    bool isAlive() const { return alive; }
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }

    // Position helpers
    sf::Vector2f getPosition() const { return position; }
    void setPosition(sf::Vector2f pos) { position = pos; }

    // Velocity helpers
    sf::Vector2f getVelocity() const { return velocity; }
    void setVelocity(sf::Vector2f vel) { velocity = vel; }

protected:
    sf::Vector2f position;
    sf::Vector2f velocity;

    float health    = 100.f;
    float maxHealth = 100.f;
    bool  alive     = true;
};
