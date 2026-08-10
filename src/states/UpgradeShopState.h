#pragma once

#include "core/StateManager.h"
#include <SFML/Graphics.hpp>
#include <optional>
class Player;
class UpgradeShopState : public GameState
{
public:
    UpgradeShopState(Game& game, Player& player);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void onEnter() override;

private:
    Player& player;
    sf::Font font;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> hpText;
    std::optional<sf::Text> damageText;
    std::optional<sf::Text> speedText;
    std::optional<sf::Text> backText;
    std::optional<sf::Text> moneyText;
    std::optional<sf::Text> messageText;

    int selectedButton = 0;

    void updateSelection();
};
