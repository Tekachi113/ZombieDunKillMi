#include "AmmoPickup.h"
#include <cmath>
#include <iostream>

AmmoPickup::AmmoPickup(sf::Vector2f pos, int amount)
    : Entity(pos)
    , refillAmount(amount)
{
    health = maxHealth = 1.f; // not meant to be damaged, just walked over

    if (texture.loadFromFile("assets/textures/pickups/ammo.png")) {
        sprite.emplace(texture);
        sf::Vector2u size = texture.getSize();
        sprite->setOrigin({ size.x * 0.5f, size.y * 0.5f });
        sprite->setPosition(position);
        sprite->setScale({ 2.2f, 2.2f }); // source art is tiny (32x22), scale up
        hasTexture = true;
    }
    else {
        std::cout << "[AmmoPickup] No texture at assets/textures/pickups/ammo.png -- using placeholder shape\n";
    }
}

void AmmoPickup::update(float dt) {
    bobTimer += dt;

    if (hasTexture && sprite) {
        float bob = std::sin(bobTimer * 3.f) * 3.f;
        sprite->setPosition({ position.x, position.y + bob });
    }
}

void AmmoPickup::render(sf::RenderTarget& target) {
    if (hasTexture && sprite) {
        target.draw(*sprite);
        return;
    }

    // Placeholder: small yellow diamond with a gentle bob, used only if
    // the texture file above is missing.
    float bob = std::sin(bobTimer * 3.f) * 3.f;

    sf::RectangleShape shape({ RADIUS * 1.6f, RADIUS * 1.6f });
    shape.setOrigin({ RADIUS * 0.8f, RADIUS * 0.8f });
    shape.setPosition({ position.x, position.y + bob });
    shape.setRotation(sf::degrees(45.f));
    shape.setFillColor(sf::Color(230, 200, 60));
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(1.f);
    target.draw(shape);
}

sf::FloatRect AmmoPickup::getBounds() const {
    return sf::FloatRect{
        { position.x - RADIUS, position.y - RADIUS },
        { RADIUS * 2.f, RADIUS * 2.f }
    };
}