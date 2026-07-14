#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

// Forward declaration
class Game;

// Base class for all game states
class GameState {
public:
    GameState(Game& game) : game(game) {}
    virtual ~GameState() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;

    virtual void onEnter() {}
    virtual void onExit() {}

protected:
    Game& game;
};

// Stack-based state manager
class StateManager {
public:
    void pushState(std::unique_ptr<GameState> state);
    void popState();
    void changeState(std::unique_ptr<GameState> state);

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderTarget& target);

    bool isEmpty() const { return states.empty(); }
    GameState* current() const;

private:
    std::vector<std::unique_ptr<GameState>> states;
};
