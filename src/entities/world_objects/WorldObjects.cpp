#include "WorldObjects.h"
#include <iostream>

// ---- BreakableBox ---------------------------------------

// ---- BreakableBox ---------------------------------------

BreakableBox::BreakableBox(sf::Vector2f pos, const sf::Texture& tex)
    : Entity(pos)
    , sprite(tex)
{
    health    = 3.f;  // hits to break
    maxHealth = 3.f;

    sprite.setScale({3.f, 3.f});
    sf::Vector2u size = tex.getSize();
    sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});
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
    std::cout << "[BreakableBox] Broken — TODO: spawn loot pickup\n";
}

// ---- ExplodingBarrel ------------------------------------

ExplodingBarrel::ExplodingBarrel(sf::Vector2f pos, const sf::Texture& tex)
    : Entity(pos)
    , sprite(tex)
{
    health    = 1.f;  // explodes on first hit
    maxHealth = 1.f;

    sprite.setScale({3.f, 3.f});
    sf::Vector2u size = tex.getSize();
    sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});
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
    std::cout << "[ExplodingBarrel] BOOM — TODO: AoE damage + particle explosion\n";
}

// ---- SceneryObject --------------------------------------

SceneryObject::SceneryObject(sf::Vector2f pos, const sf::Texture& tex, bool collidable)
    : Entity(pos)
    , sprite(tex)
    , collidable(collidable)
{
    health    = 9999.f; // Invulnerable
    maxHealth = 9999.f;

    sprite.setScale({3.f, 3.f});
    sf::Vector2u size = tex.getSize();
    sprite.setOrigin({size.x * 0.5f, size.y * 0.5f});
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
    health    = 9999.f;
    maxHealth = 9999.f;

    if (!frames.empty()) {
        sprite.emplace(frames[0]);
        sprite->setScale({3.f, 3.f});
        auto sz = frames[0].getSize();
        sprite->setOrigin({sz.x * 0.5f, sz.y * 0.5f});
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
