#include "Weapon.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>

AmmoType ammoTypeFromString(const std::string& s) {
    if (s == "small")  return AmmoType::Small;
    if (s == "medium") return AmmoType::Medium;
    if (s == "big")    return AmmoType::Big;
    return AmmoType::None;
}

Weapon::Weapon(std::string n, float dmg, float rate,
    int magSize, int reserve, float reload,
    float spr, AmmoType ammo)
    : name(std::move(n))
    , damage(dmg)
    , fireRate(rate)
    , magazineSize(magSize)
    , currentAmmo(magSize)
    , reserveAmmo(reserve)
    , reloadTime(reload)
    , spread(spr)
    , ammoType(ammo)
{}

void Weapon::update(float dt) {
    fireTimer += dt;

    if (reloading) {
        reloadTimer += dt;
        if (reloadTimer >= reloadTime) {
            int needed  = magazineSize - currentAmmo;
            int fromRes = std::min(needed, reserveAmmo);
            currentAmmo += fromRes;
            reserveAmmo -= fromRes;
            reloading   = false;
            reloadTimer = 0.f;
        }
    }

    // Advance attack animation
    if (playingAttack && !attackTextures.empty()) {
        attackTimer += dt;
        if (attackTimer >= ATTACK_FRAME_TIME) {
            attackTimer = 0.f;
            ++attackFrame;
            if (attackFrame >= static_cast<int>(attackTextures.size())) {
                playingAttack = false;
                attackFrame   = 0;
            }
        }
    }
}

void Weapon::startReload() {
    if (reloading)                       return;
    if (ammoType == AmmoType::None)      return;
    if (currentAmmo >= magazineSize)     return;
    if (reserveAmmo <= 0)                return;
    reloading   = true;
    reloadTimer = 0.f;
}

float Weapon::getReloadProgress() const {
    if (!reloading || reloadTime <= 0.f) return 0.f;
    return std::clamp(reloadTimer / reloadTime, 0.f, 1.f);
}

bool Weapon::canFire() const {
    if (reloading) return false;
    if (fireTimer < 1.f / fireRate) return false;
    if (ammoType != AmmoType::None && currentAmmo <= 0) return false;
    return true;
}

void Weapon::consumeShot(int rounds) {
    fireTimer = 0.f;
    if (ammoType != AmmoType::None)
        currentAmmo = std::max(0, currentAmmo - rounds);
}

void Weapon::autoReloadIfEmpty() {
    if (ammoType != AmmoType::None && currentAmmo <= 0 && reserveAmmo > 0)
        startReload();
}

sf::Vector2f Weapon::applySpread(sf::Vector2f dir, float spreadDeg) {
    if (spreadDeg <= 0.f) return dir;
    float half   = spreadDeg * 0.5f;
    float randDeg = -half + (static_cast<float>(std::rand()) / RAND_MAX) * spreadDeg;
    float rad     = randDeg * 3.14159265f / 180.f;
    float cs = std::cos(rad), sn = std::sin(rad);
    return { dir.x * cs - dir.y * sn, dir.x * sn + dir.y * cs };
}

// ---- Sprite / animation ------------------------------------------------

bool Weapon::loadTexture(const std::string& path) {
    if (!idleTexture.loadFromFile(path)) {
        std::cerr << "[Weapon] Cannot load sprite: " << path << "\n";
        return false;
    }
    idleTexture.setSmooth(false);
    idleSprite.emplace(idleTexture);
    sf::Vector2u sz = idleTexture.getSize();
    idleSprite->setOrigin(sf::Vector2f{ sz.x * 0.5f, sz.y * 0.5f });
    return true;
}

bool Weapon::loadAttackFrames(const std::vector<std::string>& paths) {
    attackTextures.resize(paths.size());
    attackSprites.reserve(paths.size());
    for (std::size_t i = 0; i < paths.size(); ++i) {
        if (!attackTextures[i].loadFromFile(paths[i])) {
            std::cerr << "[Weapon] Cannot load attack frame: " << paths[i] << "\n";
            return false;
        }
        attackTextures[i].setSmooth(false);
        sf::Sprite sp(attackTextures[i]);
        sf::Vector2u sz = attackTextures[i].getSize();
        sp.setOrigin(sf::Vector2f{ sz.x * 0.5f, sz.y * 0.5f });
        attackSprites.push_back(std::move(sp));
    }
    return true;
}

void Weapon::triggerAttackAnim() {
    if (!attackTextures.empty()) {
        playingAttack = true;
        attackFrame   = 0;
        attackTimer   = 0.f;
    }
}

void Weapon::drawAt(sf::RenderTarget& target,
                    sf::Vector2f handPos,
                    float angleDeg,
                    float scale) const {
    // Choose which sprite to draw
    sf::Sprite* spr = nullptr;

    if (playingAttack && attackFrame < static_cast<int>(attackSprites.size())) {
        spr = &attackSprites[attackFrame];
    } else if (idleSprite) {
        spr = &(*idleSprite);
    }

    if (!spr) return;

    spr->setPosition(handPos);
    spr->setRotation(sf::degrees(angleDeg));
    spr->setScale({ scale, scale });
    target.draw(*spr);
}