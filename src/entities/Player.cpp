#include "Player.h"
#include "../core/InputManager.h"
#include "weapons/Weapon.h"
#include "../world/EntityManager.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <cmath>

Player::Player(sf::Vector2f pos)
    : Entity(pos)
{
    health = 100.f;
    maxHealth = 100.f;
}
Player::~Player() = default;

namespace {
    std::string toLower(const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return out;
    }
}

void Player::loadAnimations(const std::string& walkFramesDir) {
    for (auto& df : dirFrames) df = DirectionFrames{};

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
        std::filesystem::path fp(p);
        std::string lower = toLower(fp.filename().string());

        Direction dir;
        if (lower.find("north") != std::string::npos)      dir = Direction::North;
        else if (lower.find("south") != std::string::npos) dir = Direction::South;
        else if (lower.find("east") != std::string::npos)  dir = Direction::East;
        else if (lower.find("west") != std::string::npos)  dir = Direction::West;
        else {
            std::cerr << "[Player] Skipping frame with no recognizable direction: " << p << "\n";
            continue;
        }

        // Check movement_2 / movement_1 before generic "idle" to avoid mismatches.
        bool isMove2 = lower.find("movement_2") != std::string::npos || lower.find("movement2") != std::string::npos;
        bool isMove1 = !isMove2 && (lower.find("movement_1") != std::string::npos || lower.find("movement1") != std::string::npos);
        bool isIdle = !isMove1 && !isMove2 && lower.find("idle") != std::string::npos;

        if (!isMove1 && !isMove2 && !isIdle) {
            std::cerr << "[Player] Skipping frame with no recognizable pose: " << p << "\n";
            continue;
        }

        sf::Texture tex;
        if (!tex.loadFromFile(p)) {
            std::cerr << "[Player] Failed to load: " << p << "\n";
            continue;
        }
        tex.setSmooth(false);

        DirectionFrames& df = framesFor(dir);
        if (isIdle)      df.idle = std::move(tex);
        else if (isMove1) df.move1 = std::move(tex);
        else              df.move2 = std::move(tex);
        df.loaded = true;
    }

    // Spawn state: facing South, Idle.
    const DirectionFrames& south = framesFor(Direction::South);
    if (south.loaded) {
        sprite.emplace(south.idle);
        sprite->setScale({ 3.f, 3.f });
        auto sz = south.idle.getSize();
        sprite->setOrigin({ sz.x * 0.5f, sz.y * 0.5f });
        sprite->setPosition(position);
        std::cout << "[Player] Animations loaded from: " << walkFramesDir << "\n";
    }
    else {
        std::cerr << "[Player] No South frames found in: " << walkFramesDir << "\n";
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

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.f) dir /= len;

    moving = (len > 0.f);
    velocity = dir * moveSpeed;
    position += velocity * dt;

    if (moving) {
        bool vertical = (pressingUp != pressingDown);   // exactly one of W/S
        bool horizontal = (pressingLeft != pressingRight); // exactly one of A/D

        // Diagonal input: vertical takes priority over horizontal.
        // (Swap the order below if you'd rather horizontal win diagonals.)
        if (vertical)
            currentDirection = pressingUp ? Direction::North : Direction::South;
        else if (horizontal)
            currentDirection = pressingLeft ? Direction::West : Direction::East;
        // else: keys cancel out (e.g. W+S) -> keep currentDirection unchanged
    }
    // When not moving, currentDirection is simply left as-is (last moving direction).
}

void Player::applyTexture(const sf::Texture& tex) {
    if (!sprite) return;
    sprite->setTexture(tex);
    auto sz = tex.getSize();
    sprite->setOrigin({ sz.x * 0.5f, sz.y * 0.5f });
}

void Player::update(float dt) {
    if (Weapon* w = getCurrentWeapon()) {
        w->update(dt);
    }

    if (sprite) {
        const DirectionFrames& df = framesFor(currentDirection);

        if (df.loaded) {
            if (moving) {
                animTimer += dt;
                if (animTimer >= animSpeed) {
                    animTimer = 0.f;
                    currentFrame = (currentFrame + 1) % 2; // toggle move1 / move2
                    applyTexture(currentFrame == 0 ? df.move1 : df.move2);
                }
            }
            else {
                currentFrame = 0;
                animTimer = 0.f;
                applyTexture(df.idle);
            }
        }

        sprite->setScale({ 3.f, 3.f });
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

    float angle = 0.f;
    switch (currentDirection) {
        case Direction::East:  angle = 0.f;   break;
        case Direction::South: angle = 90.f;  break;
        case Direction::West:  angle = 180.f; break;
        case Direction::North: angle = 270.f; break;
    }
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