#include "GameOverState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "core/Game.h"

GameOverState::GameOverState(Game& game, int finalScore)
    : GameState(game)
    , finalScore(finalScore)
{
}

void GameOverState::onEnter()
{
    if (!font.openFromFile("assets/fonts/default.ttf"))
        return;

    titleText.emplace(font);
    titleText->setString("GAME OVER");
    titleText->setCharacterSize(56);
    titleText->setFillColor(sf::Color::Red);
    titleText->setPosition({ 450.f, 150.f });
    
    scoreText.emplace(font);
    scoreText->setString("Final Score: " + std::to_string(finalScore));
    scoreText->setCharacterSize(30);
    scoreText->setFillColor(sf::Color::White);
    scoreText->setPosition({ 480.f, 220.f });

    retryText.emplace(font);
    retryText->setString("Retry");
    retryText->setCharacterSize(30);
    retryText->setPosition({ 540.f, 300.f });

    menuText.emplace(font);
    menuText->setString("Main Menu");
    menuText->setCharacterSize(30);
    menuText->setPosition({ 500.f, 360.f });

    updateSelection();
}

void GameOverState::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key->code)
        {
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::W:
            selectedButton = 0;
            updateSelection();
            break;

        case sf::Keyboard::Key::Down:
        case sf::Keyboard::Key::S:
            selectedButton = 1;
            updateSelection();
            break;

        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::Space:

            if (selectedButton == 0)
            {
                game.getStateManager().changeState(
                    std::make_unique<PlayState>(game));
            }
            else
            {
                game.getStateManager().changeState(
                    std::make_unique<MenuState>(game));
            }
            break;

        case sf::Keyboard::Key::Escape:
            game.getStateManager().changeState(
                std::make_unique<MenuState>(game));
            break;

        default:
            break;
        }
    }
}

void GameOverState::update(float dt)
{
}

void GameOverState::render(sf::RenderTarget& target)
{
    target.clear(sf::Color(20, 20, 20));

    if (titleText) target.draw(*titleText);
    if (retryText) target.draw(*retryText);
    if (menuText) target.draw(*menuText);
    if (scoreText) target.draw(*scoreText);
}

void GameOverState::updateSelection()
{
    if (retryText)
        retryText->setFillColor(selectedButton == 0 ? sf::Color::Yellow : sf::Color::White);

    if (menuText)
        menuText->setFillColor(selectedButton == 1 ? sf::Color::Yellow : sf::Color::White);
}