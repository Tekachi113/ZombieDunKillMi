#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
class Player;

class HUD
{
public:
    HUD();

    bool load();

    void update(const Player& player);

    void render(sf::RenderTarget& target);

private:
    sf::Font font;

    std::optional<sf::Text> pauseHint;
    std::optional<sf::Text> moneyText;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> weaponText;
    std::optional<sf::Text> ammoText;



    sf::RectangleShape hpBack;
    sf::RectangleShape hpFront;
};
