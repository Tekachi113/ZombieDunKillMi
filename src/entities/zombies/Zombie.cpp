#include "Zombie.h"
#include "../../world/EntityManager.h"
#include "../Projectile.h"
#include <iostream>
#include <cmath>
#include <memory>

// ---- Static shared state (single-player game) -----------
Entity* Zombie::target = nullptr;
EntityManager* Zombie::entityManagerRef = nullptr;


// ---- Zombie base ----------------------------------------

Zombie::Zombie(sf::Vector2f pos, float hp, float spd, float dmg)
    : Entity(pos)
    , moveSpeed(spd)
    , damage(dmg)
{
    health    = hp;
    maxHealth = hp;
}

void Zombie::update(float dt) {
    attackTimer += dt;

    if (!target || !target->isAlive()) return;

    sf::Vector2f diff = target->getPosition() - position;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (dist <= meleeRange) {
        attack(*target);
    }
    else {
        chase(target->getPosition(), dt);
    }
}

void Zombie::render(sf::RenderTarget& target) {
    // Placeholder: coloured circle
    sf::CircleShape circle(RADIUS);
    circle.setOrigin({RADIUS, RADIUS});
    circle.setPosition(position);
    circle.setFillColor(sf::Color(0, 180, 0));
    target.draw(circle);
}

sf::FloatRect Zombie::getBounds() const {
    return sf::FloatRect{
        { position.x - RADIUS, position.y - RADIUS },
        { RADIUS * 2.f, RADIUS * 2.f }
    };
}

void Zombie::chase(sf::Vector2f targetPos, float dt) {
    sf::Vector2f diff = targetPos - position;
    float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (len > 0.f) {
        sf::Vector2f dir = diff / len;
        position += dir * moveSpeed * dt;
    }
}

void Zombie::attack(Entity& target) {
    if (attackTimer >= 1.f / attackRate) {
        target.takeDamage(damage);
        attackTimer = 0.f;
    }
}

void Zombie::onDeath() {
    std::cout << "[Zombie] Died at (" << position.x << ", " << position.y << ")\n";
}

// ---- Subclasses -----------------------------------------

SmallZombie::SmallZombie(sf::Vector2f pos)
    : Zombie(pos, 40.f, 90.f, 8.f)
{
    xpReward    = 10;
    moneyReward = 2;
    attackRate  = 1.0f;
}

MediumZombie::MediumZombie(sf::Vector2f pos)
    : Zombie(pos, 100.f, 55.f, 15.f)
{
    xpReward    = 20;
    moneyReward = 4;
    attackRate  = 0.9f;
}

BigZombie::BigZombie(sf::Vector2f pos)
    : Zombie(pos, 280.f, 30.f, 35.f)
{
    xpReward    = 50;
    moneyReward = 8;
    attackRate  = 0.5f;
}

TurretZombie::TurretZombie(sf::Vector2f pos)
    : Zombie(pos, 150.f, 10.f, 12.f)
{
    xpReward = 35;
    moneyReward = 5;
    attackRate = 0.4f;
    spitDamage = damage; // reuse the base "damage" stat for the projectile
    meleeRange = 0.f;    // turret never melees — it only ever spits
}

void TurretZombie::update(float dt) {
    attackTimer += dt;
    spitTimer += dt;

    if (!target || !target->isAlive()) return;

    sf::Vector2f diff = target->getPosition() - position;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // Keep roughly `preferredRange` away: close in if too far, back off if too close.
    if (dist > preferredRange) {
        chase(target->getPosition(), dt);
    }
    else if (dist < preferredRange * 0.7f && dist > 0.001f) {
        sf::Vector2f awayDir = -diff / dist;
        position += awayDir * moveSpeed * dt;
    }

    // Spit whenever the target is roughly in range, on its own cooldown
    if (dist <= preferredRange * 1.3f && spitTimer >= spitCooldown) {
        spitTimer = 0.f;
        spit(target->getPosition());
    }
}

void TurretZombie::spit(sf::Vector2f targetPos) {
    if (!entityManagerRef) return; // EntityManager not wired up yet — nothing to spawn into

    sf::Vector2f dir = targetPos - position;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.001f) return;
    dir /= len;

    entityManagerRef->add(std::make_unique<Projectile>(position, dir, spitSpeed, spitDamage, this));
}