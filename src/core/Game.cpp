#include "Game.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Zombie Don't Kill Me", sf::Style::Close | sf::Style::Titlebar)
{
    window.setFramerateLimit(0); 
    window.setVerticalSyncEnabled(false);


    std::cout << "[Game] Window created: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;
    std::cout << "[Game] Render scale: " << RENDER_SCALE << "x (tile: " << TILE_SIZE << "px -> " << SCALED_TILE << "px)" << std::endl;
}

void Game::run() {
    sf::Clock clock;
    float accumulator = 0.f;

    while (window.isOpen()) {
        float frameTime = clock.restart().asSeconds();

    
        if (frameTime > 0.25f) {
            frameTime = 0.25f;
        }

        accumulator += frameTime;

  
        processEvents();

     
        while (accumulator >= TIME_STEP) {
            input.update();
            update(TIME_STEP);
            accumulator -= TIME_STEP;
        }

       
        render();

       
        if (stateManager.isEmpty()) {
            window.close();
        }
    }
}

void Game::processEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        stateManager.handleEvent(*event);
    }
}

void Game::update(float dt) {
    stateManager.update(dt);
}

void Game::render() {
    window.clear(sf::Color(30, 30, 35));
    stateManager.render(window);
    window.display();
}
