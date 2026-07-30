#pragma once

#include "core/StateManager.h"
#include <SFML/Graphics.hpp>
#include <optional>

class UpgradeShopState : public GameState
{
public:
    explicit UpgradeShopState(Game& game);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void onEnter() override;

private:
    sf::Font font;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> hpText;
    std::optional<sf::Text> damageText;
    std::optional<sf::Text> speedText;
    std::optional<sf::Text> backText;

    int selectedButton = 0;

    void updateSelection();
};
