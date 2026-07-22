#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class InputManager;

// =========================================================
//  Player — top-down WASD movement, mouse-aim
// =========================================================
class Player : public Entity {
public:
    explicit Player(sf::Vector2f pos = {400.f, 300.f});

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

    void addMoney(int amount)    { money += amount; }
    void addScore(int amount)    { score += amount; }
    void addHealth(float amount);

    static constexpr int   WEAPON_SLOTS = 4;
    static constexpr float RADIUS       = 12.f;

private:
    // Walk animation
    std::vector<sf::Texture>  walkFrames;
    std::optional<sf::Sprite> sprite;
    int   currentFrame = 0;
    float animTimer    = 0.f;
    float animSpeed    = 0.10f; // seconds per frame

    // State
    float        moveSpeed  = 160.f;
    sf::Vector2f aimDir     = {1.f, 0.f};  // toward mouse cursor (used for shooting)
    bool         moving     = false;
    bool         movingHorizontal = false;
    bool         facingLeft = false;       // set by A/D keys

    // Stats
    int money = 0;
    int score = 0;

    void renderPlaceholder(sf::RenderTarget& target);
};
