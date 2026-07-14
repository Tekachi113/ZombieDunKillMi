#pragma once

#include "core/StateManager.h"
#include <SFML/Graphics.hpp>

class MenuState : public GameState {
public:
    MenuState(Game& game);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void onEnter() override;

private:
    sf::Font font;
    std::optional<sf::Text> titleText;
    std::optional<sf::Text> subtitleText;
    std::optional<sf::Text> startText;
    std::optional<sf::Text> quitText;

    // Animated zombie sprite for the background
    std::optional<sf::Sprite> zombieSprite;
    std::vector<sf::Texture> zombieFrames;
    int currentFrame = 0;
    float animTimer = 0.f;
    float animSpeed = 0.15f;
    float zombieX = 0.f;

    // Blinking "Press ENTER" effect
    float blinkTimer = 0.f;
    bool showStartText = true;

    // Button hover
    int selectedButton = 0; // 0 = start, 1 = quit
};
