#include "ZombieFactory.h"
#include "Zombie.h"
#include <iostream>

std::unique_ptr<Zombie> ZombieFactory::create(const std::string& type, sf::Vector2f pos) {
    if (type == "small")  return std::make_unique<SmallZombie>(pos);
    if (type == "medium") return std::make_unique<MediumZombie>(pos);
    if (type == "big")    return std::make_unique<BigZombie>(pos);
    if (type == "turret") return std::make_unique<TurretZombie>(pos);

    std::cerr << "[ZombieFactory] Unknown zombie type: " << type << " — defaulting to SmallZombie\n";
    return std::make_unique<SmallZombie>(pos);
}
