#include "UpgradeShopState.h"
#include "PlayState.h"
#include "core/Game.h"
#include <memory>


UpgradeShopState::UpgradeShopState(Game& game)
    : GameState(game)
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

    hpText.emplace(font, "Increase HP", 32);
    hpText->setPosition({460.f, 250.f});

    damageText.emplace(font, "Increase Damage", 32);
    damageText->setPosition({460.f, 310.f});

    speedText.emplace(font, "Increase Speed", 32);
    speedText->setPosition({460.f, 370.f});

    backText.emplace(font, "Back", 32);
    backText->setPosition({460.f, 430.f});

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

            if (selectedButton == 3)
            {
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
}

void UpgradeShopState::render(sf::RenderTarget& target)
{
    target.clear(sf::Color(30, 30, 30));

    if (titleText) target.draw(*titleText);
    if (hpText) target.draw(*hpText);
    if (damageText) target.draw(*damageText);
    if (speedText) target.draw(*speedText);
    if (backText) target.draw(*backText);
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