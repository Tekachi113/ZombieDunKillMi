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
    // Set once at startup (same pattern as setTarget/setEntityManager)
    // so chase() can raycast against walls, not just other entities.
    static void setTileMap(const class TileMap* map) { tileMapRef = map; }
    static Entity*        getTarget()        { return target; }
    static EntityManager* getEntityManager() { return entityManagerRef; }

protected:
    float moveSpeed;
    float damage;
    int   xpReward = 10;
    int   moneyReward = 2;
    float attackTimer = 0.f;
    float attackRate = 1.f;

    float meleeRange = RADIUS + 20.f; // how close the target must be to get attacked

    // ---- Line-of-sight obstacle avoidance (see chase()/hasLineOfSight() in .cpp) ----
    // No real pathfinding here -- when the straight line to the target
    // is blocked, commit to sidestepping around ONE side until line-of-
    // sight opens back up (i.e. until it can go straight/diagonally at
    // the target again), instead of flip-flopping every frame.
    bool  avoidingObstacle = false;
    bool  sideStepRight = false;
    float avoidProgressCheckTimer = 0.f;
    float avoidDistAtLastCheck = -1.f;

    static constexpr float AVOID_PROBE_DIST = 40.f;  // how far to probe left/right when first blocked
    static constexpr float AVOID_RECHECK_INTERVAL = 0.4f;  // how often to confirm the chosen side is working
    static constexpr float AVOID_PROGRESS_MIN = 6.f;   // px expected to close in that time

    bool hasLineOfSight(sf::Vector2f from, sf::Vector2f to) const;

    static Entity* target;
    static EntityManager* entityManagerRef;
    static const class TileMap* tileMapRef;

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