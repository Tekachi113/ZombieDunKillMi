#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

class InputManager;
class EntityManager;
class Weapon;

// =========================================================
//  Player — top-down WASD movement, mouse-aim
// =========================================================
class Player : public Entity {
public:
    explicit Player(sf::Vector2f pos = { 400.f, 300.f });
    
    ~Player() override; 

    // Call every frame from PlayState, BEFORE update()
    void handleInput(const InputManager& input, float dt);

    // Pass the mouse world-position so the player can face the cursor
    void setAimTarget(sf::Vector2f worldMousePos);

    // Load walk frames from a folder
    void loadAnimations(const std::string& walkFramesDir);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

    // Accessors
    int          getMoney()     const { return money; }
    int          getScore()     const { return score; }
    float        getMoveSpeed() const { return moveSpeed; }
    sf::Vector2f getAimDir()    const { return aimDir; }   // unit vec toward cursor (for shooting)
    bool         isMoving()     const { return moving; }
    bool         isFacingLeft() const { return facingLeft; }

    void addMoney(int amount) { money += amount; }
    void addScore(int amount) { score += amount; }
    void addHealth(float amount);

    // --- Weapons ---------------------------------------------------
    // Give the player its starting loadout (or replace it wholesale,
    // e.g. after an upgrade-shop purchase reorders the slots).
    void setWeapons(std::vector<std::unique_ptr<Weapon>> loadout);

    // Call every frame from PlayState, alongside handleInput(). Reads
    // number keys (switch), R (reload), and left mouse (fire) and acts
    // on the currently equipped weapon.
    void handleCombat(const InputManager& input, EntityManager& entities);

    void switchWeapon(int slotIndex); // 0-based, clamped to WEAPON_SLOTS
    Weapon* getCurrentWeapon();
    const Weapon* getCurrentWeapon() const;
    int           getCurrentWeaponSlot() const { return currentWeaponSlot; }

    static constexpr int   WEAPON_SLOTS = 4;
    static constexpr float RADIUS = 12.f;

private:
    // Walk animation
    std::vector<sf::Texture>  walkFrames;
    std::optional<sf::Sprite> sprite;
    int   currentFrame = 0;
    float animTimer = 0.f;
    float animSpeed = 0.10f; // seconds per frame

    // State
    float        moveSpeed = 160.f;
    sf::Vector2f aimDir = { 1.f, 0.f };  // toward mouse cursor (used for shooting)
    bool         moving = false;
    bool         movingHorizontal = false;
    bool         facingLeft = false;       // set by A/D keys

    // Stats
    int money = 0;
    int score = 0;

    // Weapons
    std::vector<std::unique_ptr<Weapon>> weapons;
    int currentWeaponSlot = 0;

    void renderPlaceholder(sf::RenderTarget& target);
};