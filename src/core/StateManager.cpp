#include "StateManager.h"
#include <iostream>

void StateManager::pushState(std::unique_ptr<GameState> state) {
    if (!states.empty()) {
        states.back()->onExit();
    }
    states.push_back(std::move(state));
    states.back()->onEnter();
    std::cout << "[StateManager] State pushed. Stack size: " << states.size() << std::endl;
}

void StateManager::popState() {
    if (!states.empty()) {
        states.back()->onExit();
        states.pop_back();
        if (!states.empty()) {
            states.back()->onEnter();
        }
        std::cout << "[StateManager] State popped. Stack size: " << states.size() << std::endl;
    }
}

void StateManager::changeState(std::unique_ptr<GameState> state) {
    // Remove all existing states
    while (!states.empty()) {
        states.back()->onExit();
        states.pop_back();
    }
    // Push new state
    states.push_back(std::move(state));
    states.back()->onEnter();
    std::cout << "[StateManager] State changed. Stack size: " << states.size() << std::endl;
}

void StateManager::handleEvent(const sf::Event& event) {
    if (!states.empty()) {
        states.back()->handleEvent(event);
    }
}

void StateManager::update(float dt) {
    if (!states.empty()) {
        states.back()->update(dt);
    }
}

void StateManager::render(sf::RenderTarget& target) {
    // Render all states bottom-to-top (for overlay support like pause screen)
    for (auto& state : states) {
        state->render(target);
    }
}

GameState* StateManager::current() const {
    if (states.empty()) return nullptr;
    return states.back().get();
}
