#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>

// =========================================================
//  BreakableBox — destructible obstacle
// =========================================================
class BreakableBox : public Entity {
public:
    BreakableBox(sf::Vector2f pos, const sf::Texture& tex);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;
    void takeDamage(float amount) override;
    void onDeath() override;

private:
    sf::Sprite sprite;
};

// =========================================================
//  ExplodingBarrel — explodes when shot
// =========================================================
class ExplodingBarrel : public Entity {
public:
    ExplodingBarrel(sf::Vector2f pos, const sf::Texture& tex);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;
    void takeDamage(float amount) override;
    void onDeath() override;

    static constexpr float BLAST_RADIUS  = 100.f;
    static constexpr float BLAST_DAMAGE  = 120.f;

private:
    sf::Sprite sprite;
};

// =========================================================
//  SceneryObject — non-destructible obstacle/graphic (tree, bush, etc)
// =========================================================
class SceneryObject : public Entity {
public:
    SceneryObject(sf::Vector2f pos, const sf::Texture& tex, bool collidable = true);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

private:
    sf::Sprite sprite;
    bool collidable;
};

