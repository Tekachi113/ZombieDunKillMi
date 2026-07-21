#include "ParticleSystem.h"
#include <cstdlib>
#include <cmath>

static float randFloat(float lo, float hi) {
    return lo + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}

// ---- Public API ----------------------------------------

void ParticleSystem::emit(sf::Vector2f position,
                           const std::string& effectType,
                           int count) {
    if      (effectType == "blood")     emitBlood(position, count);
    else if (effectType == "explosion") emitExplosion(position, count);
    else if (effectType == "sparks")    emitSparks(position, count);
    else if (effectType == "smoke")     emitSmoke(position, count);
}

void ParticleSystem::emitCustom(sf::Vector2f position,
                                 sf::Vector2f baseVelocity,
                                 float spread,
                                 sf::Color color,
                                 float lifetime,
                                 float radius,
                                 int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position    = position;
        p.velocity    = baseVelocity + sf::Vector2f{
            randFloat(-spread, spread),
            randFloat(-spread, spread)
        };
        p.color       = color;
        p.lifetime    = lifetime;
        p.maxLifetime = lifetime;
        p.radius      = radius;
        particles.push_back(p);
    }
}

// ---- Update / Render -----------------------------------

void ParticleSystem::update(float dt) {
    for (auto& p : particles) {
        p.lifetime -= dt;
        p.position += p.velocity * dt;
        // Slow down over time
        p.velocity *= 0.92f;
    }
    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle& p) { return p.lifetime <= 0.f; }),
        particles.end());
}

void ParticleSystem::render(sf::RenderTarget& target) const {
    for (const auto& p : particles) {
        float alpha = std::max(0.f, p.lifetime / p.maxLifetime);
        sf::CircleShape circle(p.radius);
        circle.setOrigin({p.radius, p.radius});
        circle.setPosition(p.position);
        sf::Color c = p.color;
        c.a = static_cast<std::uint8_t>(alpha * 255.f);
        circle.setFillColor(c);
        target.draw(circle);
    }
}

// ---- Effect presets ------------------------------------

void ParticleSystem::emitBlood(sf::Vector2f pos, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position    = pos;
        float angle   = randFloat(0.f, 6.2831f);
        float speed   = randFloat(50.f, 180.f);
        p.velocity    = { std::cos(angle) * speed, std::sin(angle) * speed };
        p.color       = sf::Color(randFloat(0.5f,1.f) > 0.7f
                            ? sf::Color(180, 0, 0) : sf::Color(120, 0, 0));
        p.lifetime    = randFloat(0.3f, 0.7f);
        p.maxLifetime = p.lifetime;
        p.radius      = randFloat(2.f, 5.f);
        particles.push_back(p);
    }
}

void ParticleSystem::emitExplosion(sf::Vector2f pos, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position    = pos;
        float angle   = randFloat(0.f, 6.2831f);
        float speed   = randFloat(80.f, 300.f);
        p.velocity    = { std::cos(angle) * speed, std::sin(angle) * speed };
        // Orange/yellow fireball colours
        p.color       = (std::rand() % 2 == 0)
                        ? sf::Color(255, 140, 0)
                        : sf::Color(255, 220, 50);
        p.lifetime    = randFloat(0.2f, 0.6f);
        p.maxLifetime = p.lifetime;
        p.radius      = randFloat(4.f, 10.f);
        particles.push_back(p);
    }
    // Add smoke on top
    emitSmoke(pos, count / 2);
}

void ParticleSystem::emitSparks(sf::Vector2f pos, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position    = pos;
        float angle   = randFloat(0.f, 6.2831f);
        float speed   = randFloat(100.f, 250.f);
        p.velocity    = { std::cos(angle) * speed, std::sin(angle) * speed };
        p.color       = sf::Color(255, 230, 80);
        p.lifetime    = randFloat(0.1f, 0.4f);
        p.maxLifetime = p.lifetime;
        p.radius      = randFloat(1.f, 3.f);
        particles.push_back(p);
    }
}

void ParticleSystem::emitSmoke(sf::Vector2f pos, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position    = pos;
        p.velocity    = { randFloat(-20.f, 20.f), randFloat(-60.f, -20.f) };
        p.color       = sf::Color(80, 80, 80, 200);
        p.lifetime    = randFloat(0.5f, 1.2f);
        p.maxLifetime = p.lifetime;
        p.radius      = randFloat(5.f, 14.f);
        particles.push_back(p);
    }
}
