#include "ThrownGrenade.h"
#include "../../world/EntityManager.h"
#include <cmath>

ThrownGrenade::ThrownGrenade(sf::Vector2f pos, sf::Vector2f direction, float throwSpeed,
    float fuseTime, float blastRad, float blastDmg,
    EntityManager& ents, Entity* ownerEntity)
    : Entity(pos)
    , fuseTimer(fuseTime)
    , blastRadius(blastRad)
    , blastDamage(blastDmg)
    , entities(ents)
    , owner(ownerEntity)
{
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f dir = (len > 0.0001f) ? direction / len : sf::Vector2f{ 1.f, 0.f };
    setVelocity(dir * throwSpeed);
}

void ThrownGrenade::update(float dt) {
    if (!alive) return;

    // Lobbed feel: the grenade decelerates as it "arcs" through the air
    // instead of sliding forever at a constant speed.
    setVelocity(getVelocity() * 0.94f);
    position += getVelocity() * dt;

    fuseTimer -= dt;
    if (fuseTimer <= 0.f) {
        explode();
    }
}

void ThrownGrenade::render(sf::RenderTarget& target) {
    if (!alive) return;

    // TODO: swap for the pack's grenade sprite / spinning animation (Person B, art pass)
    sf::CircleShape shape(RADIUS);
    shape.setOrigin({ RADIUS, RADIUS });
    shape.setPosition(position);
    shape.setFillColor(sf::Color(60, 90, 40));
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(1.f);
    target.draw(shape);
}

sf::FloatRect ThrownGrenade::getBounds() const {
    return sf::FloatRect({ position.x - RADIUS, position.y - RADIUS }, { RADIUS * 2.f, RADIUS * 2.f });
}

void ThrownGrenade::explode() {
    if (exploded) return;
    exploded = true;
    alive = false;

    // AoE damage to everything caught in the blast, excluding the thrower
    // (no self-damage — keeps grenades usable at close range for the player).
    for (Entity* target : entities.queryRadius(position, blastRadius)) {
        if (target == this || target == owner) continue;
        if (!target->isAlive()) continue;
        target->takeDamage(blastDamage);
    }

    // TODO: ParticleSystem::emit("explosion", position) once Person A's
    // ParticleSystem is wired up — shares the animation with ExplodingBarrel.
}