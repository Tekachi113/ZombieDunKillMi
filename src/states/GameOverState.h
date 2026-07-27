#pragma once

#include "core/StateManager.h"
#include <SFML/Graphics.hpp>
#include <optional>

class GameOverState : public GameState
{
public:
    explicit GameOverState(Game& game);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void onEnter() override;

private:
    sf::Font font;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> retryText;
    std::optional<sf::Text> menuText;

    int selectedButton = 0;

    void updateSelection();
};
