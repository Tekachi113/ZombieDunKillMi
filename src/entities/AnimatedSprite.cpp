#include "AnimatedSprite.h"

// ---------------- Animation ----------------

void Animation::update(float dt) {
    if (frames.size() <= 1 || finished) return;

    timer += dt;
    while (timer >= frameDuration) {
        timer -= frameDuration;
        ++currentFrame;

        if (currentFrame >= frames.size()) {
            if (looping) {
                currentFrame = 0;
            }
            else {
                currentFrame = frames.size() - 1;
                finished = true;
                break;
            }
        }
    }
}

void Animation::reset() {
    currentFrame = 0;
    timer = 0.f;
    finished = false;
}

const sf::Texture* Animation::getCurrentTexture() const {
    if (frames.empty()) return nullptr;
    return frames[currentFrame];
}

// ---------------- AnimatedSprite ----------------

AnimatedSprite::AnimatedSprite(sf::Texture& initialTexture)
    : sprite(initialTexture)
{
}

void AnimatedSprite::addAnimation(const std::string& name, Animation anim) {
    animations[name] = std::move(anim);
}

void AnimatedSprite::play(const std::string& name, bool restart) {
    if (name == currentAnimation && !restart) return;

    auto it = animations.find(name);
    if (it == animations.end()) return; // unknown animation name, ignore

    currentAnimation = name;
    it->second.reset();

    if (const sf::Texture* tex = it->second.getCurrentTexture()) {
        sprite.setTexture(*tex, true);
    }
}

void AnimatedSprite::update(float dt) {
    if (currentAnimation.empty()) return;

    auto it = animations.find(currentAnimation);
    if (it == animations.end()) return;

    it->second.update(dt);

    if (const sf::Texture* tex = it->second.getCurrentTexture()) {
        sprite.setTexture(*tex, true);
    }
}

bool AnimatedSprite::isCurrentAnimationFinished() const {
    auto it = animations.find(currentAnimation);
    return it == animations.end() || it->second.isFinished();
}
