#include "Zombie.h"
#include "../../world/EntityManager.h"
#include "../../world/TileMap.h"
#include "../Projectile.h"
#include "../world_objects/WorldObjects.h"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

// ---- Static shared state (single-player game) -----------
Entity* Zombie::target = nullptr;
EntityManager* Zombie::entityManagerRef = nullptr;
const TileMap* Zombie::tileMapRef = nullptr;

namespace {
    // Same "what counts as solid" rule CollisionSystem uses, kept local
    // here since hasLineOfSight() only needs a yes/no answer, not the
    // full collision-resolution logic.
    bool isSolidObstacle(Entity* e) {
        if (dynamic_cast<BreakableBox*>(e) || dynamic_cast<ExplodingBarrel*>(e)) {
            return true;
        }
        if (auto* scen = dynamic_cast<SceneryObject*>(e)) {
            return scen->getBounds().size.x > 0.f;
        }
        return false;
    }
}


// ---- Zombie base ----------------------------------------

Zombie::Zombie(sf::Vector2f pos, float hp, float spd, float dmg)
    : Entity(pos)
    , moveSpeed(spd)
    , damage(dmg)
{
    health = hp;
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
    circle.setOrigin({ RADIUS, RADIUS });
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

bool Zombie::hasLineOfSight(sf::Vector2f from, sf::Vector2f to) const {
    sf::Vector2f diff = to - from;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (dist < 1.f) return true;

    sf::Vector2f dir = diff / dist;
    const float step = 12.f; // sample interval along the ray
    int steps = static_cast<int>(dist / step);

    // Gather solid obstacles once instead of re-querying EntityManager
    // for every sample point along the ray.
    std::vector<Entity*> obstacles;
    if (entityManagerRef) {
        for (Entity* e : entityManagerRef->getAll()) {
            if (e == this || !e->isAlive()) continue;
            if (isSolidObstacle(e)) obstacles.push_back(e);
        }
    }

    for (int i = 1; i < steps; ++i) {
        sf::Vector2f p = from + dir * (step * static_cast<float>(i));

        if (tileMapRef && !tileMapRef->isWalkable(p)) return false;

        for (Entity* obs : obstacles) {
            if (obs->getBounds().contains(p)) return false;
        }
    }
    return true;
}

void Zombie::chase(sf::Vector2f targetPos, float dt) {
    sf::Vector2f diff = targetPos - position;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (dist < 1.f) return;

    sf::Vector2f dir = diff / dist;
    sf::Vector2f moveDir;

    if (hasLineOfSight(position, targetPos)) {
        // Clear line to the target -- go straight/diagonally at it and
        // drop whatever sidestep we were committed to.
        moveDir = dir;
        avoidingObstacle = false;
        avoidDistAtLastCheck = -1.f;
    }
    else {
        if (!avoidingObstacle) {
            // Just became blocked -- pick a side ONCE and commit to it
            // until line-of-sight opens back up, instead of
            // flip-flopping every frame.
            sf::Vector2f perpR(-dir.y, dir.x);
            sf::Vector2f perpL(dir.y, -dir.x);
            bool rightClear = !tileMapRef || tileMapRef->isWalkable(position + perpR * AVOID_PROBE_DIST);
            bool leftClear = !tileMapRef || tileMapRef->isWalkable(position + perpL * AVOID_PROBE_DIST);
            if (rightClear != leftClear) sideStepRight = rightClear;
            // if both/neither probe is clear, keep whatever side we
            // last used (the periodic recheck below will flip it if
            // this side turns out to be a dead end)

            avoidingObstacle = true;
            avoidDistAtLastCheck = -1.f;
            avoidProgressCheckTimer = 0.f;
        }

        // While committed to avoiding, periodically confirm the chosen
        // side is actually making progress; if this side is also a
        // dead end, flip to the other one instead of pushing forever.
        avoidProgressCheckTimer += dt;
        if (avoidProgressCheckTimer >= AVOID_RECHECK_INTERVAL) {
            if (avoidDistAtLastCheck >= 0.f &&
                (avoidDistAtLastCheck - dist) < AVOID_PROGRESS_MIN) {
                sideStepRight = !sideStepRight;
            }
            avoidDistAtLastCheck = dist;
            avoidProgressCheckTimer = 0.f;
        }

        sf::Vector2f perp = sideStepRight ? sf::Vector2f(-dir.y, dir.x)
            : sf::Vector2f(dir.y, -dir.x);
        // Mostly sideways with a bit of forward bias so it still
        // advances around the obstacle instead of pure strafing.
        moveDir = perp * 0.85f + dir * 0.35f;
        float len = std::sqrt(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
        if (len > 0.0001f) moveDir /= len;
    }

    position += moveDir * moveSpeed * dt;
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
    xpReward = 10;
    moneyReward = 2;
    attackRate = 1.0f;
}

MediumZombie::MediumZombie(sf::Vector2f pos)
    : Zombie(pos, 100.f, 55.f, 15.f)
{
    xpReward = 20;
    moneyReward = 4;
    attackRate = 0.9f;
}

BigZombie::BigZombie(sf::Vector2f pos)
    : Zombie(pos, 280.f, 30.f, 35.f)
{
    xpReward = 50;
    moneyReward = 8;
    attackRate = 0.5f;
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