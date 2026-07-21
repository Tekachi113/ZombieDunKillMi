#include "Entity.h"
#include <algorithm>

Entity::Entity(sf::Vector2f pos)
    : position(pos)
{}

void Entity::takeDamage(float amount) {
    if (!alive) return;
    health -= amount;
    if (health <= 0.f) {
        health = 0.f;
        alive  = false;
        onDeath();
    }
}
