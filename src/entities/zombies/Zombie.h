#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>

class EntityManager;
// =========================================================
//  Zombie — base class for all zombie types
// =========================================================
class Zombie : public Entity {
public:
    Zombie(sf::Vector2f pos, float hp, float speed, float damage);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

    virtual void chase(sf::Vector2f targetPos, float dt);
    virtual void attack(Entity& target);
    void onDeath() override;

    int getXpReward()    const { return xpReward; }
    int getMoneyReward() const { return moneyReward; }

    static void setTarget(Entity* t) { target = t; }
    static void setEntityManager(EntityManager* em) { entityManagerRef = em; }
    static Entity* getTarget() { return target; }

protected:
    float moveSpeed;
    float damage;
    int   xpReward = 10;
    int   moneyReward = 2;
    float attackTimer = 0.f;
    float attackRate = 1.f;

    float meleeRange = RADIUS + 20.f; // how close the target must be to get attacked

    // ---- Obstacle-stuck detection (see chase() in .cpp) ----
    // No real pathfinding here -- these let chase() notice when it's
    // making no progress toward the target (blocked by an obstacle)
    // and sidestep around it instead of vibrating in place forever.
    float stuckCheckTimer = 0.f;
    float distAtLastCheck = -1.f;
    float stuckFor = 0.f;
    bool  sideStepRight = false;

    static constexpr float STUCK_CHECK_INTERVAL = 0.35f; // how often to sample progress
    static constexpr float STUCK_PROGRESS_MIN = 8.f;   // px expected to close in that time
    static constexpr float STUCK_TRIGGER_TIME = 0.7f;  // how long "no progress" before sidestepping

    static Entity* target;
    static EntityManager* entityManagerRef;

    static constexpr float RADIUS = 14.f;
};

// ---- Subclasses ----

class SmallZombie : public Zombie {
public:
    explicit SmallZombie(sf::Vector2f pos);
};

class MediumZombie : public Zombie {
public:
    explicit MediumZombie(sf::Vector2f pos);
};

class BigZombie : public Zombie {
public:
    explicit BigZombie(sf::Vector2f pos);
};

class TurretZombie : public Zombie {
public:
    explicit TurretZombie(sf::Vector2f pos);
    void update(float dt) override;
protected:
    void spit(sf::Vector2f targetPos);

    float spitTimer = 0.f;
    float spitCooldown = 2.5f;
    float preferredRange = 250.f;
    float spitDamage;
    float spitSpeed = 260.f;
};