#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>

// Forward declarations
class ResourceManager;

// =========================================================
//  Player — stub header (full implementation: Person B)
// =========================================================
class Player : public Entity {
public:
    explicit Player(sf::Vector2f pos = {0.f, 0.f});

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

    // Stat accessors needed by other systems
    int   getMoney()   const { return money; }
    int   getScore()   const { return score; }
    float getMoveSpeed() const { return moveSpeed; }

    // Called by pickup system
    void addMoney(int amount)  { money  += amount; }
    void addScore(int amount)  { score  += amount; }
    void addHealth(float amount);

    // Weapon slot count (for HUD)
    static constexpr int WEAPON_SLOTS = 4;

private:
    float moveSpeed = 200.f;
    int   money     = 0;
    int   score     = 0;
};
