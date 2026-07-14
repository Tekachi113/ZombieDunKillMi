#include "MenuState.h"
#include "core/Game.h"
#include <iostream>
#include <cmath>

MenuState::MenuState(Game& game)
    : GameState(game)
{
}

void MenuState::onEnter() {
    std::cout << "[MenuState] Entering menu state" << std::endl;

    // Load a basic font — we'll use SFML's built-in approach
    // For now, create a fallback font. The actual game will load a pixel font.
    if (!font.loadFromFile("assets/fonts/default.ttf")) {
        std::cerr << "[MenuState] Warning: Could not load font, text won't render" << std::endl;
    }

    // Title text
    titleText.setFont(font);
    titleText.setString("ZOMBIE DON'T KILL ME");
    titleText.setCharacterSize(56);
    titleText.setFillColor(sf::Color(200, 30, 30));
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(3.f);
    titleText.setStyle(sf::Text::Bold);
    // Center horizontally
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f, titleBounds.top + titleBounds.height / 2.f);
    titleText.setPosition(Game::WINDOW_WIDTH / 2.f, 180.f);

    // Subtitle text
    subtitleText.setFont(font);
    subtitleText.setString("A Zombie Survival Shooter");
    subtitleText.setCharacterSize(20);
    subtitleText.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect subBounds = subtitleText.getLocalBounds();
    subtitleText.setOrigin(subBounds.left + subBounds.width / 2.f, subBounds.top + subBounds.height / 2.f);
    subtitleText.setPosition(Game::WINDOW_WIDTH / 2.f, 240.f);

    // Start text
    startText.setFont(font);
    startText.setString("> START GAME <");
    startText.setCharacterSize(28);
    startText.setFillColor(sf::Color(255, 220, 100));
    startText.setOutlineColor(sf::Color::Black);
    startText.setOutlineThickness(2.f);
    sf::FloatRect startBounds = startText.getLocalBounds();
    startText.setOrigin(startBounds.left + startBounds.width / 2.f, startBounds.top + startBounds.height / 2.f);
    startText.setPosition(Game::WINDOW_WIDTH / 2.f, 400.f);

    // Quit text
    quitText.setFont(font);
    quitText.setString("QUIT");
    quitText.setCharacterSize(24);
    quitText.setFillColor(sf::Color(150, 150, 150));
    quitText.setOutlineColor(sf::Color::Black);
    quitText.setOutlineThickness(1.f);
    sf::FloatRect quitBounds = quitText.getLocalBounds();
    quitText.setOrigin(quitBounds.left + quitBounds.width / 2.f, quitBounds.top + quitBounds.height / 2.f);
    quitText.setPosition(Game::WINDOW_WIDTH / 2.f, 460.f);

    // Load zombie walk animation frames for background
    const std::string zombiePath = "assets/textures/zombies/big/";
    for (int i = 0; i < 9; ++i) {
        sf::Texture tex;
        // Try to load frames — naming from tileset: Zombie-Tileset---_0412_Capa-413.png ... _0420_Capa-421.png
        std::string filename = zombiePath + "walk_" + std::to_string(i) + ".png";
        if (tex.loadFromFile(filename)) {
            tex.setSmooth(false);
            zombieFrames.push_back(std::move(tex));
        }
    }

    if (!zombieFrames.empty()) {
        zombieSprite.setTexture(zombieFrames[0]);
        zombieSprite.setScale(static_cast<float>(Game::RENDER_SCALE * 2), static_cast<float>(Game::RENDER_SCALE * 2));
        zombieSprite.setPosition(-100.f, static_cast<float>(Game::WINDOW_HEIGHT) - 200.f);
        zombieX = -100.f;
    }
}

void MenuState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                selectedButton = 0;
                break;
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                selectedButton = 1;
                break;
            case sf::Keyboard::Return:
            case sf::Keyboard::Space:
                if (selectedButton == 0) {
                    std::cout << "[MenuState] Start game!" << std::endl;
                    // TODO: Change to PlayState when it's ready
                } else if (selectedButton == 1) {
                    game.getWindow().close();
                }
                break;
            case sf::Keyboard::Escape:
                game.getWindow().close();
                break;
            default:
                break;
        }
    }
}

void MenuState::update(float dt) {
    // Blink the start text
    blinkTimer += dt;
    if (blinkTimer >= 0.5f) {
        showStartText = !showStartText;
        blinkTimer = 0.f;
    }

    // Update button colors based on selection
    if (selectedButton == 0) {
        startText.setFillColor(sf::Color(255, 220, 100));
        quitText.setFillColor(sf::Color(150, 150, 150));
    } else {
        startText.setFillColor(sf::Color(150, 150, 150));
        quitText.setFillColor(sf::Color(255, 100, 100));
    }

    // Animate zombie walking across screen
    if (!zombieFrames.empty()) {
        animTimer += dt;
        if (animTimer >= animSpeed) {
            currentFrame = (currentFrame + 1) % static_cast<int>(zombieFrames.size());
            zombieSprite.setTexture(zombieFrames[currentFrame]);
            animTimer = 0.f;
        }

        zombieX += 40.f * dt;
        if (zombieX > static_cast<float>(Game::WINDOW_WIDTH) + 100.f) {
            zombieX = -100.f;
        }
        zombieSprite.setPosition(zombieX, static_cast<float>(Game::WINDOW_HEIGHT) - 200.f);
    }
}

void MenuState::render(sf::RenderTarget& target) {
    // Dark atmospheric background
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(Game::WINDOW_WIDTH), static_cast<float>(Game::WINDOW_HEIGHT)));
    bg.setFillColor(sf::Color(20, 15, 15));
    target.draw(bg);

    // Draw a subtle red vignette overlay
    sf::RectangleShape vignette(sf::Vector2f(static_cast<float>(Game::WINDOW_WIDTH), static_cast<float>(Game::WINDOW_HEIGHT)));
    vignette.setFillColor(sf::Color(80, 0, 0, 40));
    target.draw(vignette);

    // Draw zombie in background
    if (!zombieFrames.empty()) {
        target.draw(zombieSprite);
    }

    // Draw a ground line
    sf::RectangleShape ground(sf::Vector2f(static_cast<float>(Game::WINDOW_WIDTH), 4.f));
    ground.setPosition(0.f, static_cast<float>(Game::WINDOW_HEIGHT) - 100.f);
    ground.setFillColor(sf::Color(60, 50, 40));
    target.draw(ground);

    // Draw title
    target.draw(titleText);
    target.draw(subtitleText);

    // Draw menu buttons
    if (selectedButton == 0 && showStartText) {
        target.draw(startText);
    } else if (selectedButton != 0) {
        target.draw(startText);
    }
    target.draw(quitText);

    // Draw credits at bottom
    sf::Text credits;
    credits.setFont(font);
    credits.setString("Art: Zombie Apocalypse Tileset by Ittai Manero");
    credits.setCharacterSize(14);
    credits.setFillColor(sf::Color(100, 100, 100));
    sf::FloatRect credBounds = credits.getLocalBounds();
    credits.setOrigin(credBounds.left + credBounds.width / 2.f, credBounds.top + credBounds.height / 2.f);
    credits.setPosition(Game::WINDOW_WIDTH / 2.f, Game::WINDOW_HEIGHT - 30.f);
    target.draw(credits);
}
