#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>

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

protected:
    float moveSpeed;
    float damage;
    int   xpReward    = 10;
    int   moneyReward = 2;
    float attackTimer = 0.f;
    float attackRate  = 1.f;

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
    float spitTimer      = 0.f;
    float spitCooldown   = 2.5f;
    float preferredRange = 250.f;
};
