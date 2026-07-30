#pragma once

#include "../Entity.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

class Player;
class EntityManager;
class ParticleSystem;

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

    // Set once (single-player game, same pattern as Zombie::setTarget)
    // so broken boxes can hand money straight to the player. Whoever
    // wires PlayState needs to call this once, right next to the
    // existing Zombie::setTarget(&player) call.
    static void setPlayer(Player* p) { playerRef = p; }

private:
    sf::Sprite sprite;
    static Player* playerRef;

    int moneyDropMin = 1;
    int moneyDropMax = 3;
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

    // Set once at startup so exploding barrels can deal AoE damage and
    // spawn a visual explosion. Same pattern as Zombie::setEntityManager.
    static void setEntityManager(EntityManager* em) { entityManagerRef = em; }
    static void setParticleSystem(ParticleSystem* ps) { particleSystemRef = ps; }

    static constexpr float BLAST_RADIUS = 100.f;
    static constexpr float BLAST_DAMAGE = 120.f;

private:
    sf::Sprite sprite;
    static EntityManager* entityManagerRef;
    static ParticleSystem* particleSystemRef;
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
        float animSpeed = 0.15f,
        bool  collidable = false);

    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    sf::FloatRect getBounds() const override;

private:
    // frames MUST be declared before sprite so it is initialised first
    std::vector<sf::Texture>  frames;
    std::optional<sf::Sprite> sprite;   // emplaced once frames are ready
    int   currentFrame = 0;
    float animTimer = 0.f;
    float animSpeed;
    bool  collidable;
};