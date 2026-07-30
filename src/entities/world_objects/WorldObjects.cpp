#include "WorldObjects.h"
#include "../Player.h"
#include "../../world/EntityManager.h"
#include "../../world/ParticleSystem.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

// ---- BreakableBox ---------------------------------------

Player* BreakableBox::playerRef = nullptr;

BreakableBox::BreakableBox(sf::Vector2f pos, const sf::Texture& tex)
    : Entity(pos)
    , sprite(tex)
{
    health = 3.f;  // hits to break
    maxHealth = 3.f;

    sprite.setScale({ 3.f, 3.f });
    sf::Vector2u size = tex.getSize();
    sprite.setOrigin({ size.x * 0.5f, size.y * 0.5f });
    sprite.setPosition(pos);
}

void BreakableBox::update(float /*dt*/) {
}

void BreakableBox::render(sf::RenderTarget& target) {
    target.draw(sprite);
}

sf::FloatRect BreakableBox::getBounds() const {
    const sf::Texture& tex = sprite.getTexture();
    float w = tex.getSize().x * 3.f;
    float h = tex.getSize().y * 3.f;
    return sf::FloatRect{
        { position.x - w * 0.5f, position.y - h * 0.5f },
        { w, h }
    };
}

void BreakableBox::takeDamage(float amount) {
    Entity::takeDamage(amount);
}

void BreakableBox::onDeath() {
    if (playerRef) {
        int amount = moneyDropMin + (std::rand() % (moneyDropMax - moneyDropMin + 1));
        playerRef->addMoney(amount);
        std::cout << "[BreakableBox] Broken — dropped $" << amount << "\n";
    }
    else {
        // setPlayer() was never called at startup — box still breaks
        // fine, it just can't hand out money. Not a crash, just no loot.
        std::cout << "[BreakableBox] Broken — no player reference set, no loot given\n";
    }
    // NOTE: this grants money instantly on break. Once entities/pickups/
    // (Person C) exists, swap this for spawning an actual Money pickup
    // entity at `position` so the player has to walk over to collect it.
}

// ---- ExplodingBarrel ------------------------------------

EntityManager* ExplodingBarrel::entityManagerRef = nullptr;
ParticleSystem* ExplodingBarrel::particleSystemRef = nullptr;

ExplodingBarrel::ExplodingBarrel(sf::Vector2f pos, const sf::Texture& tex)
    : Entity(pos)
    , sprite(tex)
{
    health = 1.f;  // explodes on first hit
    maxHealth = 1.f;

    sprite.setScale({ 3.f, 3.f });
    sf::Vector2u size = tex.getSize();
    sprite.setOrigin({ size.x * 0.5f, size.y * 0.5f });
    sprite.setPosition(pos);
}

void ExplodingBarrel::update(float /*dt*/) {
}

void ExplodingBarrel::render(sf::RenderTarget& target) {
    target.draw(sprite);
}

sf::FloatRect ExplodingBarrel::getBounds() const {
    const sf::Texture& tex = sprite.getTexture();
    float w = tex.getSize().x * 3.f;
    float h = tex.getSize().y * 3.f;
    return sf::FloatRect{
        { position.x - w * 0.5f, position.y - h * 0.5f },
        { w, h }
    };
}

void ExplodingBarrel::takeDamage(float amount) {
    Entity::takeDamage(amount);
}

void ExplodingBarrel::onDeath() {
    std::cout << "[ExplodingBarrel] BOOM at (" << position.x << ", " << position.y << ")\n";

    if (particleSystemRef) {
        particleSystemRef->emit(position, "explosion", 24);
    }

    if (entityManagerRef) {
        for (Entity* e : entityManagerRef->getAll()) {
            if (e == this || !e->isAlive()) continue;

            sf::Vector2f diff = e->getPosition() - position;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (dist > BLAST_RADIUS) continue;

            // Linear falloff: full damage at the center, ~0 at the edge.
            // If this hits another ExplodingBarrel for lethal damage,
            // Entity::takeDamage() calls its onDeath() too -> automatic
            // chain reaction, no extra code needed.
            float falloff = 1.f - (dist / BLAST_RADIUS);
            e->takeDamage(BLAST_DAMAGE * falloff);
        }
    }
    else {
        std::cout << "[ExplodingBarrel] No EntityManager reference set — "
            "explosion is visual only, no AoE damage\n";
    }
}

// ---- SceneryObject --------------------------------------

SceneryObject::SceneryObject(sf::Vector2f pos, const sf::Texture& tex, bool collidable)
    : Entity(pos)
    , sprite(tex)
    , collidable(collidable)
{
    health = 9999.f; // Invulnerable
    maxHealth = 9999.f;

    sprite.setScale({ 3.f, 3.f });
    sf::Vector2u size = tex.getSize();
    sprite.setOrigin({ size.x * 0.5f, size.y * 0.5f });
    sprite.setPosition(pos);
}

void SceneryObject::update(float /*dt*/) {
}

void SceneryObject::render(sf::RenderTarget& target) {
    target.draw(sprite);
}

sf::FloatRect SceneryObject::getBounds() const {
    if (collidable) {
        const sf::Texture& tex = sprite.getTexture();
        float w = tex.getSize().x * 3.f;
        float h = tex.getSize().y * 3.f;
        return sf::FloatRect{
            { position.x - w * 0.5f, position.y - h * 0.5f },
            { w, h }
        };
    }
    // Return a tiny/null rect if it's not collidable
    return sf::FloatRect{};
}

// ---- AnimatedScenery ------------------------------------

AnimatedScenery::AnimatedScenery(sf::Vector2f pos,
    std::vector<sf::Texture> framesIn,
    float animSpeedIn,
    bool  collidableIn)
    : Entity(pos)
    , frames(std::move(framesIn))
    , animSpeed(animSpeedIn)
    , collidable(collidableIn)
{
    health = 9999.f;
    maxHealth = 9999.f;

    if (!frames.empty()) {
        sprite.emplace(frames[0]);
        sprite->setScale({ 3.f, 3.f });
        auto sz = frames[0].getSize();
        sprite->setOrigin({ sz.x * 0.5f, sz.y * 0.5f });
        sprite->setPosition(pos);
    }
}

void AnimatedScenery::update(float dt) {
    if (frames.size() <= 1 || !sprite) return;
    animTimer += dt;
    if (animTimer >= animSpeed) {
        animTimer = 0.f;
        currentFrame = (currentFrame + 1) % static_cast<int>(frames.size());
        sprite->setTexture(frames[currentFrame]);
    }
}

void AnimatedScenery::render(sf::RenderTarget& target) {
    if (sprite) target.draw(*sprite);
}

sf::FloatRect AnimatedScenery::getBounds() const {
    if (collidable && sprite && !frames.empty()) {
        auto sz = frames[0].getSize();
        float w = sz.x * 3.f;
        float h = sz.y * 3.f;
        return sf::FloatRect{
            { position.x - w * 0.5f, position.y - h * 0.5f },
            { w, h }
        };
    }
    return sf::FloatRect{};
}