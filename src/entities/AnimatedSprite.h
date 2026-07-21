#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <unordered_map>

// =========================================================
//  Animation — drives a sequence of texture pointers
// =========================================================
class Animation {
public:
    std::vector<const sf::Texture*> frames;
    float frameDuration = 0.1f;
    bool  looping       = true;

    void update(float dt);
    void reset();
    bool isFinished() const { return finished; }
    const sf::Texture* getCurrentTexture() const;

private:
    std::size_t currentFrame = 0;
    float       timer        = 0.f;
    bool        finished     = false;
};

// =========================================================
//  AnimatedSprite — wraps sf::Sprite + named Animation map
// =========================================================
class AnimatedSprite {
public:
    explicit AnimatedSprite(sf::Texture& initialTexture);

    void addAnimation(const std::string& name, Animation anim);
    void play(const std::string& name, bool restart = false);
    void update(float dt);

    bool isCurrentAnimationFinished() const;

    sf::Sprite& getSprite() { return sprite; }
    const sf::Sprite& getSprite() const { return sprite; }

private:
    sf::Sprite  sprite;
    std::unordered_map<std::string, Animation> animations;
    std::string currentAnimation;
};
