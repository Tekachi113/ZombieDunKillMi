#include "Game.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Zombie Don't Kill Me", sf::Style::Close | sf::Style::Titlebar)
{
    window.setFramerateLimit(0); // We handle timing ourselves
    window.setVerticalSyncEnabled(false);

    // Set the window icon could be done here if we have an icon file

    std::cout << "[Game] Window created: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;
    std::cout << "[Game] Render scale: " << RENDER_SCALE << "x (tile: " << TILE_SIZE << "px -> " << SCALED_TILE << "px)" << std::endl;
}

void Game::run() {
    sf::Clock clock;
    float accumulator = 0.f;

    while (window.isOpen()) {
        float frameTime = clock.restart().asSeconds();

        // Clamp to prevent spiral of death
        if (frameTime > 0.25f) {
            frameTime = 0.25f;
        }

        accumulator += frameTime;

        // Process events every frame
        processEvents();

        // Fixed timestep updates
        while (accumulator >= TIME_STEP) {
            input.update();
            update(TIME_STEP);
            accumulator -= TIME_STEP;
        }

        // Render every frame
        render();

        // Exit if state stack is empty
        if (stateManager.isEmpty()) {
            window.close();
        }
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return;
        }

        stateManager.handleEvent(event);
    }
}

void Game::update(float dt) {
    stateManager.update(dt);
}

void Game::render() {
    window.clear(sf::Color(30, 30, 35)); // Dark background
    stateManager.render(window);
    window.display();
}
