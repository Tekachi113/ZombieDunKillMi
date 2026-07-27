#include "PauseState.h"
#include "MenuState.h"
#include "core/Game.h"

PauseState::PauseState(Game& game)
    : GameState(game)
{
}
void PauseState::onEnter()
{
    if (!font.openFromFile("assets/fonts/default.ttf")) {
        return;
    }

    titleText.emplace(font);
    titleText->setString("PAUSED");
    titleText->setCharacterSize(48);
    titleText->setFillColor(sf::Color::White);
    titleText->setPosition({ 500.f, 150.f });

    resumeText.emplace(font);
    resumeText->setString("Resume");
    resumeText->setCharacterSize(30);
    resumeText->setPosition({ 520.f, 280.f });

    menuText.emplace(font);
    menuText->setString("Main Menu");
    menuText->setCharacterSize(30);
    menuText->setPosition({ 520.f, 340.f });

    quitText.emplace(font);
    quitText->setString("Quit");
    quitText->setCharacterSize(30);
    quitText->setPosition({ 520.f, 400.f });

    updateSelection();
}

void PauseState::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key->code)
        {
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::Up:
            if (selectedButton > 0)
                selectedButton--;
            updateSelection();
            break;

        case sf::Keyboard::Key::S:
        case sf::Keyboard::Key::Down:
            if (selectedButton < 2)
                selectedButton++;
            updateSelection();
            break;

        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::Space:

            if (selectedButton == 0)
            {
                
                game.getStateManager().popState();
            }
            else if (selectedButton == 1)
            {
                
                game.getStateManager().changeState(
                    std::make_unique<MenuState>(game));
            }
            else if (selectedButton == 2)
            {
                
                game.getWindow().close();
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

void PauseState::update(float dt)
{
}

void PauseState::render(sf::RenderTarget& target)
{
    if (titleText)
        target.draw(*titleText);

    if (resumeText)
        target.draw(*resumeText);

    if (menuText)
        target.draw(*menuText);

    if (quitText)
        target.draw(*quitText);
}
void PauseState::updateSelection()
{
    if (resumeText)
        resumeText->setFillColor(selectedButton == 0 ? sf::Color::Yellow : sf::Color::White);

    if (menuText)
        menuText->setFillColor(selectedButton == 1 ? sf::Color::Yellow : sf::Color::White);

    if (quitText)
        quitText->setFillColor(selectedButton == 2 ? sf::Color::Yellow : sf::Color::White);
}