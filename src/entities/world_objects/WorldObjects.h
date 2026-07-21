#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>

// =========================================================
//  BreakableBox — stub (full implementation: Person B)
// =========================================================
class BreakableBox : public Entity {
public:
    explicit BreakableBox(sf::Vector2f pos);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;
    void takeDamage(float amount) override;
    void onDeath() override;

private:
    static constexpr float SIZE = 24.f;
};

// =========================================================
//  ExplodingBarrel — stub (full implementation: Person B)
// =========================================================
class ExplodingBarrel : public Entity {
public:
    explicit ExplodingBarrel(sf::Vector2f pos);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;
    void takeDamage(float amount) override;
    void onDeath() override;

    static constexpr float BLAST_RADIUS  = 100.f;
    static constexpr float BLAST_DAMAGE  = 120.f;

private:
    static constexpr float SIZE = 24.f;
};
