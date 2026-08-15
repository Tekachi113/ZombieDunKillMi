#include "UpgradeShopState.h"
#include "PlayState.h"
#include "core/Game.h"
#include <memory>
#include "entities/Player.h"
#include <string>
#include "entities/weapons/Weapon.h"


UpgradeShopState::UpgradeShopState(Game& game, Player& player)
    : GameState(game), player(player)
{
}

void UpgradeShopState::onEnter()
{
    if (!font.openFromFile("assets/fonts/default.ttf"))
    {
       
        return;
    }

    titleText.emplace(font, "UPGRADE SHOP", 42);
    titleText->setPosition({420.f, 120.f});

    hpText.emplace(font, "Increase HP - $20", 32);
    hpText->setPosition({460.f, 250.f});

    damageText.emplace(font, "Increase Damage - $30", 32);
    damageText->setPosition({460.f, 310.f});

    speedText.emplace(font, "Increase Speed - $25", 32);
    speedText->setPosition({460.f, 370.f});

    backText.emplace(font, "Back", 32);
    backText->setPosition({460.f, 430.f});

    moneyText.emplace(font, "Money: 0", 28);
    moneyText->setPosition({ 460.f, 500.f });

    messageText.emplace(font, "", 24);
    messageText->setPosition({ 460.f, 550.f });
    updateSelection();
}

void UpgradeShopState::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key->code)
        {
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::W:
            if (selectedButton > 0)
                selectedButton--;

            updateSelection();
            break;

        case sf::Keyboard::Key::Down:
        case sf::Keyboard::Key::S:
            if (selectedButton < 3)
                selectedButton++;

            updateSelection();
            break;

        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::Space:

            if (selectedButton == 0)
            {
                // Upgrade HP
                if (player.getMoney() >= 20)
                {
                    player.addMoney(-20);
                    player.increaseMaxHealth(20.f);
                    messageText->setString("HP upgraded!");
                }
                else
                {
                    messageText->setString("Not enough money!");
                }
            }
            else if (selectedButton == 1)
            {
                // Upgrade Damage
                if (player.getMoney() >= 30)
                {
                    Weapon* weapon = player.getCurrentWeapon();

                    if (weapon)
                    {
                        player.addMoney(-30);
                        weapon->setDamage(weapon->getDamage() + 10.f);
                        messageText->setString("Damage upgraded!");
                    }
                }
                else
                {
                    messageText->setString("Not enough money!");
                }
            }
            else if (selectedButton == 2)
            {
                // Upgrade Speed
                if (player.getMoney() >= 25)
                {
                    player.addMoney(-25);
                    player.setMoveSpeed(player.getMoveSpeed() + 20.f);
                    messageText->setString("Speed upgraded!");
                }
                else
                {
                    messageText->setString("Not enough money!");
                }
            }
            else if (selectedButton == 3)
            {
                // Back
                game.getStateManager().popState();
            }

            break;

        case sf::Keyboard::Key::Escape:

            game.getStateManager().popState();
            break;

        default:
            break;
        }
    }
}

void UpgradeShopState::update(float dt)
{
    if (moneyText)
    {
        moneyText->setString(
            "Money: " + std::to_string(player.getMoney()));
    }
}

void UpgradeShopState::render(sf::RenderTarget& target)
{
    target.clear(sf::Color(30, 30, 30));

    if (titleText) target.draw(*titleText);
    if (hpText) target.draw(*hpText);
    if (damageText) target.draw(*damageText);
    if (speedText) target.draw(*speedText);
    if (backText) target.draw(*backText);
    if (moneyText) target.draw(*moneyText);
    if (messageText) target.draw(*messageText);
     
}
void UpgradeShopState::updateSelection()
{
    hpText->setFillColor(sf::Color::White);
    damageText->setFillColor(sf::Color::White);
    speedText->setFillColor(sf::Color::White);
    backText->setFillColor(sf::Color::White);

    switch (selectedButton)
    {
    case 0:
        hpText->setFillColor(sf::Color::Yellow);
        break;

    case 1:
        damageText->setFillColor(sf::Color::Yellow);
        break;

    case 2:
        speedText->setFillColor(sf::Color::Yellow);
        break;

    case 3:
        backText->setFillColor(sf::Color::Yellow);
        break;
    }
}