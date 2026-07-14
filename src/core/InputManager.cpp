#include "InputManager.h"

void InputManager::update() {
    // Store previous frame state
    previousKeys = currentKeys;
    previousMouseButtons = currentMouseButtons;

    // Poll current keyboard state for commonly used keys
    static const sf::Keyboard::Key trackedKeys[] = {
        sf::Keyboard::Key::W, sf::Keyboard::Key::A, sf::Keyboard::Key::S, sf::Keyboard::Key::D,
        sf::Keyboard::Key::R, sf::Keyboard::Key::E, sf::Keyboard::Key::F, sf::Keyboard::Key::Q,
        sf::Keyboard::Key::Num1, sf::Keyboard::Key::Num2, sf::Keyboard::Key::Num3, sf::Keyboard::Key::Num4,
        sf::Keyboard::Key::Escape, sf::Keyboard::Key::Space, sf::Keyboard::Key::LShift, sf::Keyboard::Key::Tab
    };

    for (auto key : trackedKeys) {
        currentKeys[static_cast<int>(key)] = sf::Keyboard::isKeyPressed(key);
    }

    // Poll mouse buttons
    currentMouseButtons[static_cast<int>(sf::Mouse::Button::Left)] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    currentMouseButtons[static_cast<int>(sf::Mouse::Button::Right)] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
    currentMouseButtons[static_cast<int>(sf::Mouse::Button::Middle)] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key) const {
    auto it = currentKeys.find(static_cast<int>(key));
    return it != currentKeys.end() && it->second;
}

bool InputManager::isKeyJustPressed(sf::Keyboard::Key key) const {
    int k = static_cast<int>(key);
    auto curr = currentKeys.find(k);
    auto prev = previousKeys.find(k);
    bool currDown = (curr != currentKeys.end() && curr->second);
    bool prevDown = (prev != previousKeys.end() && prev->second);
    return currDown && !prevDown;
}

bool InputManager::isKeyJustReleased(sf::Keyboard::Key key) const {
    int k = static_cast<int>(key);
    auto curr = currentKeys.find(k);
    auto prev = previousKeys.find(k);
    bool currDown = (curr != currentKeys.end() && curr->second);
    bool prevDown = (prev != previousKeys.end() && prev->second);
    return !currDown && prevDown;
}

bool InputManager::isMouseButtonPressed(sf::Mouse::Button button) const {
    auto it = currentMouseButtons.find(static_cast<int>(button));
    return it != currentMouseButtons.end() && it->second;
}

bool InputManager::isMouseButtonJustPressed(sf::Mouse::Button button) const {
    int b = static_cast<int>(button);
    auto curr = currentMouseButtons.find(b);
    auto prev = previousMouseButtons.find(b);
    bool currDown = (curr != currentMouseButtons.end() && curr->second);
    bool prevDown = (prev != previousMouseButtons.end() && prev->second);
    return currDown && !prevDown;
}

sf::Vector2i InputManager::getMouseScreenPos(const sf::RenderWindow& window) const {
    return sf::Mouse::getPosition(window);
}

sf::Vector2f InputManager::getMouseWorldPos(const sf::RenderWindow& window) const {
    return window.mapPixelToCoords(sf::Mouse::getPosition(window));
}
