#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

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

// =========================================================
//  AnimatedScenery — looping multi-frame decoration (water, windmill, birds …)
// =========================================================
class AnimatedScenery : public Entity {
public:
    // frames: pre-loaded textures (ownership transferred via move)
    // animSpeed: seconds per frame
    // collidable: if true, getBounds() returns a solid rectangle
    AnimatedScenery(sf::Vector2f pos,
                    std::vector<sf::Texture> frames,
                    float animSpeed  = 0.15f,
                    bool  collidable = false);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

private:
    // frames MUST be declared before sprite so it is initialised first
    std::vector<sf::Texture>  frames;
    std::optional<sf::Sprite> sprite;   // emplaced once frames are ready
    int   currentFrame = 0;
    float animTimer    = 0.f;
    float animSpeed;
    bool  collidable;
};

