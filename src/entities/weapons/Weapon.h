#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>

class Entity;
class EntityManager;

enum class AmmoType { None, Small, Medium, Big };
AmmoType ammoTypeFromString(const std::string& s);

class Weapon {
public:
    Weapon(std::string name, float damage, float fireRate,
        int magazineSize, int reserveAmmo, float reloadTime,
        float spread, AmmoType ammoType);
    virtual ~Weapon() = default;

    virtual void fire(Entity& owner, sf::Vector2f origin, sf::Vector2f direction,
        EntityManager& entities) = 0;

    virtual void update(float dt);
    virtual void startReload();

    bool  isReloading()      const { return reloading; }
    float getReloadProgress()const;

    const std::string& getName()        const { return name; }
    float              getDamage()      const { return damage; }
    void               setDamage(float v)     { damage = v; }
    int                getCurrentAmmo() const { return currentAmmo; }
    int                getReserveAmmo() const { return reserveAmmo; }
    int                getMagazineSize()const { return magazineSize; }
    AmmoType           getAmmoType()    const { return ammoType; }
    void               addReserveAmmo(int n)  { reserveAmmo += n; }

    // --- Sprite / animation -------------------------------------------
    // Load the idle/default sprite from a single PNG.
    bool loadTexture(const std::string& path);

    // Load ordered attack-animation frames (e.g. knife swing).
    // Calling fire() starts playing this animation once.
    bool loadAttackFrames(const std::vector<std::string>& paths);

    // Draw weapon at handPos rotated angleDeg degrees.
    void drawAt(sf::RenderTarget& target,
                sf::Vector2f handPos,
                float angleDeg,
                float scale = 3.f) const;

    bool hasSprite() const { return !idleTexture.getSize().x == 0 || !attackTextures.empty(); }

    // Called by fire() implementations to trigger attack animation
    void triggerAttackAnim();

protected:
    bool canFire() const;
    void consumeShot(int rounds = 1);
    void autoReloadIfEmpty();
    static sf::Vector2f applySpread(sf::Vector2f dir, float spreadDeg);

    std::string name;
    float    damage;
    float    fireRate;
    int      magazineSize;
    int      currentAmmo;
    int      reserveAmmo;
    float    reloadTime;
    float    spread;
    AmmoType ammoType;

    float fireTimer   = 0.f;
    float reloadTimer = 0.f;
    bool  reloading   = false;

private:
    // Idle sprite
    sf::Texture               idleTexture;
    mutable std::optional<sf::Sprite> idleSprite;

    // Attack animation frames
    std::vector<sf::Texture>          attackTextures;
    mutable std::vector<sf::Sprite>   attackSprites;

    // Animation state
    bool  playingAttack = false;
    int   attackFrame   = 0;
    float attackTimer   = 0.f;
    static constexpr float ATTACK_FRAME_TIME = 0.07f; // seconds per frame
};