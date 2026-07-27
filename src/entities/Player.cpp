#include "Player.h"
#include "../core/InputManager.h"
#include "weapons/Weapon.h"
#include "../world/EntityManager.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>

Player::Player(sf::Vector2f pos)
    : Entity(pos)
{
    health = 100.f;
    maxHealth = 100.f;
}
Player::~Player() = default;

void Player::loadAnimations(const std::string& walkFramesDir) {
    walkFrames.clear();

    std::vector<std::string> paths;
    try {
        for (auto& entry : std::filesystem::directory_iterator(walkFramesDir)) {
            if (entry.path().extension() == ".png")
                paths.push_back(entry.path().string());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[Player] Cannot scan walk frames: " << e.what() << "\n";
    }
    std::sort(paths.begin(), paths.end());

    for (auto& p : paths) {
        sf::Texture tex;
        if (tex.loadFromFile(p)) {
            tex.setSmooth(false);
            walkFrames.push_back(std::move(tex));
        }
    }

    if (!walkFrames.empty()) {
        sprite.emplace(walkFrames[0]);
        sprite->setScale({ 3.f, 3.f });
        auto sz = walkFrames[0].getSize();
        sprite->setOrigin({ sz.x * 0.5f, sz.y * 0.5f });
        std::cout << "[Player] Loaded " << walkFrames.size() << " walk frames\n";
    }
    else {
        std::cerr << "[Player] No walk frames found in: " << walkFramesDir << "\n";
    }
}

void Player::setAimTarget(sf::Vector2f worldMousePos) {
    sf::Vector2f diff = worldMousePos - position;
    float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (len > 0.5f)
        aimDir = diff / len;
}

void Player::handleInput(const InputManager& input, float dt) {
    sf::Vector2f dir{ 0.f, 0.f };

    bool pressingLeft = input.isKeyPressed(sf::Keyboard::Key::A);
    bool pressingRight = input.isKeyPressed(sf::Keyboard::Key::D);
    bool pressingUp = input.isKeyPressed(sf::Keyboard::Key::W);
    bool pressingDown = input.isKeyPressed(sf::Keyboard::Key::S);

    if (pressingUp)    dir.y -= 1.f;
    if (pressingDown)  dir.y += 1.f;
    if (pressingLeft)  dir.x -= 1.f;
    if (pressingRight) dir.x += 1.f;

    if (pressingLeft && !pressingRight)
        facingLeft = true;
    else if (pressingRight && !pressingLeft)
        facingLeft = false;

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.f) dir /= len;

  
    movingHorizontal = (pressingLeft || pressingRight);
    moving = (len > 0.f);
    velocity = dir * moveSpeed;
    position += velocity * dt;
}


void Player::update(float dt) {
    if (Weapon* w = getCurrentWeapon()) {
        w->update(dt);
    }

    if (!walkFrames.empty() && sprite) {

        if (movingHorizontal) {
            animTimer += dt;
            if (animTimer >= animSpeed) {
                animTimer = 0.f;
                currentFrame = (currentFrame + 1) % static_cast<int>(walkFrames.size());
                sprite->setTexture(walkFrames[currentFrame]);
            }
        }
        else {

            currentFrame = 0;
            animTimer = 0.f;
            sprite->setTexture(walkFrames[0]);
        }


        float scaleX = facingLeft ? -3.f : 3.f;
        sprite->setScale({ scaleX, 3.f });
        sprite->setPosition(position);
    }
}

void Player::render(sf::RenderTarget& target) {
    if (sprite)
        target.draw(*sprite);
    else
        renderPlaceholder(target);
}

void Player::renderPlaceholder(sf::RenderTarget& target) {
    sf::CircleShape circle(RADIUS);
    circle.setOrigin({ RADIUS, RADIUS });
    circle.setPosition(position);
    circle.setFillColor(sf::Color(80, 160, 255));
    circle.setOutlineColor(sf::Color::White);
    circle.setOutlineThickness(2.f);
    target.draw(circle);

    // Draw facing direction line
    sf::RectangleShape line({ RADIUS * 1.8f, 2.f });
    line.setFillColor(sf::Color::White);
    line.setPosition(position);
    float angle = facingLeft ? 180.f : 0.f;
    line.setRotation(sf::degrees(angle));
    target.draw(line);
}

sf::FloatRect Player::getBounds() const {
    return sf::FloatRect{
        {position.x - RADIUS, position.y - RADIUS},
        {RADIUS * 2.f,        RADIUS * 2.f}
    };
}

void Player::addHealth(float amount) {
    health = std::min(health + amount, maxHealth);
}


void Player::setWeapons(std::vector<std::unique_ptr<Weapon>> loadout) {
    weapons = std::move(loadout);
    currentWeaponSlot = 0;
}

void Player::switchWeapon(int slotIndex) {
    if (weapons.empty()) return;
    slotIndex = std::clamp(slotIndex, 0, static_cast<int>(weapons.size()) - 1);
    if (!weapons[slotIndex]) return; // empty slot — ignore
    currentWeaponSlot = slotIndex;
}

Weapon* Player::getCurrentWeapon() {
    if (currentWeaponSlot < 0 || currentWeaponSlot >= static_cast<int>(weapons.size()))
        return nullptr;
    return weapons[currentWeaponSlot].get();
}

const Weapon* Player::getCurrentWeapon() const {
    if (currentWeaponSlot < 0 || currentWeaponSlot >= static_cast<int>(weapons.size()))
        return nullptr;
    return weapons[currentWeaponSlot].get();
}

void Player::handleCombat(const InputManager& input, EntityManager& entities) {
    // --- Weapon switching: number keys 1-4 map to slots 0-3 ---
    if (input.isKeyJustPressed(sf::Keyboard::Key::Num1)) switchWeapon(0);
    if (input.isKeyJustPressed(sf::Keyboard::Key::Num2)) switchWeapon(1);
    if (input.isKeyJustPressed(sf::Keyboard::Key::Num3)) switchWeapon(2);
    if (input.isKeyJustPressed(sf::Keyboard::Key::Num4)) switchWeapon(3);

    Weapon* weapon = getCurrentWeapon();
    if (!weapon) return;


    if (input.isKeyJustPressed(sf::Keyboard::Key::R)) {
        weapon->startReload();
    }

    if (input.isMouseButtonPressed(sf::Mouse::Button::Left)) {
        weapon->fire(*this, position, aimDir, entities);
    }
}