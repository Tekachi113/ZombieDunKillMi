#include "Weapon.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

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
{
}

void Weapon::update(float dt) {
    fireTimer += dt;

    if (reloading) {
        reloadTimer += dt;
        if (reloadTimer >= reloadTime) {
            int needed = magazineSize - currentAmmo;
            int fromRes = std::min(needed, reserveAmmo);
            currentAmmo += fromRes;
            reserveAmmo -= fromRes;
            reloading = false;
            reloadTimer = 0.f;
        }
    }
}

void Weapon::startReload() {
    if (reloading) return;
    if (ammoType == AmmoType::None) return;          // melee — nothing to reload
    if (currentAmmo >= magazineSize) return;          // already full
    if (reserveAmmo <= 0) return;                     // nothing left to load

    reloading = true;
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
    if (ammoType != AmmoType::None) {
        currentAmmo = std::max(0, currentAmmo - rounds);
    }
}

void Weapon::autoReloadIfEmpty() {
    if (ammoType != AmmoType::None && currentAmmo <= 0 && reserveAmmo > 0) {
        startReload();
    }
}

sf::Vector2f Weapon::applySpread(sf::Vector2f dir, float spreadDeg) {
    if (spreadDeg <= 0.f) return dir;

    float half = spreadDeg * 0.5f;
    float randDeg = -half + (static_cast<float>(std::rand()) / RAND_MAX) * spreadDeg;
    float rad = randDeg * 3.14159265f / 180.f;

    float cs = std::cos(rad), sn = std::sin(rad);
    return { dir.x * cs - dir.y * sn, dir.x * sn + dir.y * cs };
}