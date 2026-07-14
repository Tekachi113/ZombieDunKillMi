#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>

class InputManager {
public:
    void update();

    // Keyboard
    bool isKeyPressed(sf::Keyboard::Key key) const;
    bool isKeyJustPressed(sf::Keyboard::Key key) const;
    bool isKeyJustReleased(sf::Keyboard::Key key) const;

    // Mouse
    bool isMouseButtonPressed(sf::Mouse::Button button) const;
    bool isMouseButtonJustPressed(sf::Mouse::Button button) const;
    sf::Vector2i getMouseScreenPos(const sf::RenderWindow& window) const;
    sf::Vector2f getMouseWorldPos(const sf::RenderWindow& window) const;

private:
    std::unordered_map<int, bool> currentKeys;
    std::unordered_map<int, bool> previousKeys;
    std::unordered_map<int, bool> currentMouseButtons;
    std::unordered_map<int, bool> previousMouseButtons;
};
