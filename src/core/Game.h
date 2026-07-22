#pragma once

#include <SFML/Graphics.hpp>
#include "StateManager.h"
#include "ResourceManager.h"
#include "InputManager.h"

class Game {
public:
    Game();
    void run();

    StateManager& getStateManager() { return stateManager; }
    ResourceManager& getResources() { return resources; }
    InputManager& getInput() { return input; }
    sf::RenderWindow& getWindow() { return window; }

    static constexpr unsigned int WINDOW_WIDTH = 1280;
    static constexpr unsigned int WINDOW_HEIGHT = 720;
    static constexpr float TARGET_UPS = 60.f;
    static constexpr float TIME_STEP = 1.f / TARGET_UPS;
    static constexpr int RENDER_SCALE = 3;
    static constexpr int TILE_SIZE = 16;
    static constexpr int SCALED_TILE = TILE_SIZE * RENDER_SCALE; // 48

private:
    void processEvents();
    void update(float dt);
    void render();

    sf::RenderWindow window;
    StateManager stateManager;
    ResourceManager resources;
    InputManager input;
};
