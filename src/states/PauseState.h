#pragma once

#include "core/StateManager.h"
#include <SFML/Graphics.hpp>
#include <optional>

class PauseState : public GameState
{
public:
    PauseState(Game& game);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void onEnter() override;

private:
    sf::Font font;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> resumeText;
    std::optional<sf::Text> menuText;
    std::optional<sf::Text> quitText;

    int selectedButton = 0;

    void updateSelection();
};
