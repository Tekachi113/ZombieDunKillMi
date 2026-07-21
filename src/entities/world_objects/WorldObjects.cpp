#include "WorldObjects.h"
#include <iostream>

// ---- BreakableBox ---------------------------------------

BreakableBox::BreakableBox(sf::Vector2f pos)
    : Entity(pos)
{
    health    = 3.f;  // hits to break
    maxHealth = 3.f;
}

void BreakableBox::update(float /*dt*/) {
    // Static — no update logic
}

void BreakableBox::render(sf::RenderTarget& target) {
    sf::RectangleShape rect({SIZE, SIZE});
    rect.setOrigin({SIZE * 0.5f, SIZE * 0.5f});
    rect.setPosition(position);
    rect.setFillColor(sf::Color(139, 90, 43));
    rect.setOutlineColor(sf::Color(80, 50, 20));
    rect.setOutlineThickness(2.f);
    target.draw(rect);
}

sf::FloatRect BreakableBox::getBounds() const {
    return sf::FloatRect{
        { position.x - SIZE * 0.5f, position.y - SIZE * 0.5f },
        { SIZE, SIZE }
    };
}

void BreakableBox::takeDamage(float amount) {
    Entity::takeDamage(amount);
}

void BreakableBox::onDeath() {
    std::cout << "[BreakableBox] Broken — TODO: spawn loot pickup\n";
    // TODO: Person B — spawn MoneyPickup / AmmoPickup / HealthKitPickup
}

// ---- ExplodingBarrel ------------------------------------

ExplodingBarrel::ExplodingBarrel(sf::Vector2f pos)
    : Entity(pos)
{
    health    = 1.f;  // explodes on first hit
    maxHealth = 1.f;
}

void ExplodingBarrel::update(float /*dt*/) {
    // Static — no update logic
}

void ExplodingBarrel::render(sf::RenderTarget& target) {
    sf::CircleShape circle(SIZE * 0.5f);
    circle.setOrigin({SIZE * 0.5f, SIZE * 0.5f});
    circle.setPosition(position);
    circle.setFillColor(sf::Color(200, 80, 30));
    circle.setOutlineColor(sf::Color(120, 40, 10));
    circle.setOutlineThickness(2.f);
    target.draw(circle);
}

sf::FloatRect ExplodingBarrel::getBounds() const {
    return sf::FloatRect{
        { position.x - SIZE * 0.5f, position.y - SIZE * 0.5f },
        { SIZE, SIZE }
    };
}

void ExplodingBarrel::takeDamage(float amount) {
    Entity::takeDamage(amount);
}

void ExplodingBarrel::onDeath() {
    std::cout << "[ExplodingBarrel] BOOM — TODO: AoE damage + particle explosion\n";
    // TODO: CollisionSystem / PlayState: AoE damage in BLAST_RADIUS
    // TODO: ParticleSystem: emit "explosion" at position
}
