#pragma once


#include <SFML/Graphics.hpp>
#include <string>

// Forward declarations
class Entity;
class EntityManager;

// =========================================================
//  AmmoType — matches the "ammoType" strings in weapon_stats.json
// =========================================================
enum class AmmoType { None, Small, Medium, Big };

AmmoType ammoTypeFromString(const std::string& s);

// =========================================================
//  Weapon — abstract base for Knife / Pistol / Shotgun / Grenade
//
//  Subclasses implement fire(); everything else (cooldown timing,
//  ammo bookkeeping, reload timing) is handled here so each
//  subclass only has to worry about *what happens on a hit*.
// =========================================================
class Weapon {
public:
    Weapon(std::string name, float damage, float fireRate,
        int magazineSize, int reserveAmmo, float reloadTime,
        float spread, AmmoType ammoType);
    virtual ~Weapon() = default;

    // Attempt to fire/attack. Subclasses check canFire() themselves
    // (so melee weapons that ignore ammo can still fire).
    //   owner     — the entity firing (used to avoid self-hits)
    //   origin    — world position the attack originates from
    //   direction — unit vector aim direction
    //   entities  — lets ranged weapons spawn Projectiles / melee query targets
    virtual void fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
        EntityManager& entities) = 0;

    // Call once per frame while this weapon is equipped
    virtual void update(float dt);

    // Begin a reload (no-op if already full, reloading, or ammo-less like the knife)
    virtual void startReload();

    bool isReloading() const { return reloading; }
    float getReloadProgress() const; // 0 (just started) .. 1 (finished)

    const std::string& getName()        const { return name; }
    float               getDamage()      const { return damage; }
    int                 getCurrentAmmo() const { return currentAmmo; }
    int                 getReserveAmmo() const { return reserveAmmo; }
    int                 getMagazineSize()const { return magazineSize; }
    AmmoType             getAmmoType()   const { return ammoType; }

    void addReserveAmmo(int amount) { reserveAmmo += amount; }

protected:
    // Ready to fire: off cooldown, not reloading, and (if it uses ammo) has a round chambered
    bool canFire() const;

    // Consumes the cooldown + one round of ammo (call from subclass fire() on a successful shot)
    void consumeShot(int rounds = 1);

    // Auto-reload trigger: subclasses call this from fire() when the mag is empty
    void autoReloadIfEmpty();

    // Rotates `dir` by a random angle in [-spreadDeg/2, +spreadDeg/2]
    static sf::Vector2f applySpread(sf::Vector2f dir, float spreadDeg);

    std::string name;
    float damage;
    float fireRate;      // shots (or swings) per second
    int   magazineSize;
    int   currentAmmo;
    int   reserveAmmo;
    float reloadTime;
    float spread;        // degrees
    AmmoType ammoType;

    float fireTimer = 0.f; // counts UP; ready when >= 1/fireRate
    float reloadTimer = 0.f; // counts UP while reloading
    bool  reloading = false;
};