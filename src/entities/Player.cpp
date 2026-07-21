#include "Player.h"

Player::Player(sf::Vector2f pos)
    : Entity(pos)
{
    health    = 100.f;
    maxHealth = 100.f;
}

void Player::update(float /*dt*/) {
    // TODO: Person B — movement, shooting, weapon switching
}

void Player::render(sf::RenderTarget& /*target*/) {
    // TODO: Person B — animated sprite
}

sf::FloatRect Player::getBounds() const {
    constexpr float SIZE = 20.f;
    return sf::FloatRect{
        { position.x - SIZE * 0.5f, position.y - SIZE * 0.5f },
        { SIZE, SIZE }
    };
}

void Player::addHealth(float amount) {
    health = std::min(health + amount, maxHealth);
}
