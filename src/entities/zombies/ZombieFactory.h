#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

class Zombie;

// Creates a Zombie subclass by type name ("small", "medium", "big", "turret")
class ZombieFactory {
public:
    static std::unique_ptr<Zombie> create(const std::string& type, sf::Vector2f pos);
};
