#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <array>
#include <optional>
#include <string>

class InputManager;
class EntityManager;
class Weapon;


class Player : public Entity {
public:
    explicit Player(sf::Vector2f pos = { 400.f, 300.f });
    
    ~Player() override; 

    // Call every frame from PlayState, BEFORE update()
    void handleInput(const InputManager& input, float dt);

    // Pass the mouse world-position so the player can face the cursor
    void setAimTarget(sf::Vector2f worldMousePos);

    // Load walk frames from a folder. Expects filenames that contain
    // one of "north"/"south"/"east"/"west" AND one of
    // "idle"/"movement_1"/"movement_2" (case-insensitive, any prefix/suffix ok).
    void loadAnimations(const std::string& walkFramesDir);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

    // Facing direction (top-down 4-way)
    enum class Direction { North, South, East, West };

    // Accessors
    int          getMoney()     const { return money; }
    int          getScore()     const { return score; }
    float        getMoveSpeed() const { return moveSpeed; }
    sf::Vector2f getAimDir()    const { return aimDir; }   // unit vec toward cursor (for shooting)
    bool         isMoving()     const { return moving; }
    bool         isFacingLeft() const { return currentDirection == Direction::West; }
    Direction    getDirection() const { return currentDirection; }

    void addMoney(int amount) { money += amount; }
    void addScore(int amount) { score += amount; }
    void addHealth(float amount);
    void setMoveSpeed(float speed) { moveSpeed = speed; }

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
    // Per-direction animation frames
    struct DirectionFrames {
        sf::Texture idle;
        sf::Texture move1;
        sf::Texture move2;
        bool loaded = false;
    };

    static constexpr int DIRECTION_COUNT = 4;
    std::array<DirectionFrames, DIRECTION_COUNT> dirFrames;

    static int dirIndex(Direction d) { return static_cast<int>(d); }
    DirectionFrames&       framesFor(Direction d)       { return dirFrames[dirIndex(d)]; }
    const DirectionFrames& framesFor(Direction d) const { return dirFrames[dirIndex(d)]; }

    std::optional<sf::Sprite> sprite;
    int   currentFrame = 0;   // 0 = move1, 1 = move2 (toggles while moving)
    float animTimer = 0.f;
    float animSpeed = 0.10f; // seconds per frame

    Direction currentDirection = Direction::South; // spawn facing South, Idle

    void applyTexture(const sf::Texture& tex);

    // State
    float        moveSpeed = 160.f;
    sf::Vector2f aimDir = { 1.f, 0.f };  // toward mouse cursor (used for shooting)
    bool         moving = false;

    // Stats
    int money = 0;
    int score = 0;

    // Weapons
    std::vector<std::unique_ptr<Weapon>> weapons;
    int currentWeaponSlot = 0;

    void renderPlaceholder(sf::RenderTarget& target);
};