#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// =========================================================
//  Particle — a single visual particle
// =========================================================
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color    color;
    float        lifetime;      // seconds remaining
    float        maxLifetime;   // for alpha fade calculation
    float        radius;
};

// =========================================================
//  ParticleSystem — emits and renders short-lived particles
// =========================================================
class ParticleSystem {
public:
    // Emit preset effects by name
    // Supported types: "blood", "explosion", "sparks", "smoke"
    void emit(sf::Vector2f position,
              const std::string& effectType,
              int count = 8);

    // Custom emit with full control
    void emitCustom(sf::Vector2f position,
                    sf::Vector2f baseVelocity,
                    float spread,
                    sf::Color color,
                    float lifetime,
                    float radius,
                    int count);

    void update(float dt);
    void render(sf::RenderTarget& target) const;

    // Returns number of live particles (for debug HUD)
    int getParticleCount() const { return static_cast<int>(particles.size()); }

    // Remove all particles (e.g. on state change)
    void clear() { particles.clear(); }

private:
    std::vector<Particle> particles;

    // Pre-defined effect configs
    void emitBlood(sf::Vector2f pos, int count);
    void emitExplosion(sf::Vector2f pos, int count);
    void emitSparks(sf::Vector2f pos, int count);
    void emitSmoke(sf::Vector2f pos, int count);
};
