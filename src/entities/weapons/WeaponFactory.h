#pragma once

#include <memory>
#include <string>
#include <vector>

class Weapon;

// Data-driven weapon creation, reading stats from assets/data/weapon_stats.json
// (mirrors ZombieFactory's pattern for the zombie side).
class WeaponFactory {
public:
    // Loads (and caches) the JSON config. Safe to call more than once —
    // subsequent calls are no-ops unless forceReload is true.
    static bool loadConfig(const std::string& path = "assets/data/weapon_stats.json",
        bool forceReload = false);

    // Builds one weapon by key ("knife" | "pistol" | "shotgun" | "grenade").
    // Returns nullptr if the type is unknown or the config hasn't loaded.
    static std::unique_ptr<Weapon> create(const std::string& type);

    // Convenience for Player: the starting 4-weapon loadout in slot order
    // (knife, pistol, shotgun, grenade) — matches Player::WEAPON_SLOTS.
    static std::vector<std::unique_ptr<Weapon>> createDefaultLoadout();
};
