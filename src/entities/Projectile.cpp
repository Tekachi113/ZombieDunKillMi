#include "Projectile.h"
#include <cmath>

Projectile::Projectile(sf::Vector2f pos, sf::Vector2f dir,
                       float spd, float dmg, Entity* own)
    : Entity(pos)
    , direction(dir)
    , speed(spd)
    , damage(dmg)
    , owner(own)
{}

void Projectile::update(float dt) {
    position += direction * speed * dt;
    lifetime -= dt;
    if (lifetime <= 0.f) alive = false;
}

void Projectile::render(sf::RenderTarget& target) {
    sf::CircleShape circle(RADIUS);
    circle.setOrigin({RADIUS, RADIUS});
    circle.setPosition(position);
    circle.setFillColor(sf::Color::Yellow);
    target.draw(circle);
}

sf::FloatRect Projectile::getBounds() const {
    return sf::FloatRect{
        { position.x - RADIUS, position.y - RADIUS },
        { RADIUS * 2.f, RADIUS * 2.f }
    };
}

void Projectile::onHit() {
    if (!piercing) alive = false;
}
