#include "WeaponFactory.h"
#include "Weapon.h"
#include "Knife.h"
#include "Pistol.h"
#include "Shotgun.h"
#include "Grenade.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace {
    nlohmann::json g_config;
    bool g_loaded = false;
}

bool WeaponFactory::loadConfig(const std::string& path, bool forceReload) {
    if (g_loaded && !forceReload) return true;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[WeaponFactory] Cannot open weapon config: " << path << "\n";
        return false;
    }

    try { file >> g_config; }
    catch (const std::exception& e) {
        std::cerr << "[WeaponFactory] JSON error: " << e.what() << "\n";
        return false;
    }

    g_loaded = true;
    std::cout << "[WeaponFactory] Loaded weapon config from " << path << "\n";
    return true;
}

std::unique_ptr<Weapon> WeaponFactory::create(const std::string& type) {
    if (!g_loaded && !loadConfig()) return nullptr;

    if (!g_config.contains("weapons") || !g_config["weapons"].contains(type)) {
        std::cerr << "[WeaponFactory] Unknown weapon type: " << type << "\n";
        return nullptr;
    }

    const auto& w = g_config["weapons"][type];
    std::unique_ptr<Weapon> wep;

    if (type == "knife") {
        wep = std::make_unique<Knife>(
            w.value("damage", 40.f),
            w.value("attackRate", 2.0f),
            w.value("range", 60.f),
            w.value("swingArc", 90.f));
        // Knife: item_10 idle, item_10/11/12 attack animation
        wep->loadTexture("assets/textures/pickups/weapons/item_10.png");
        wep->loadAttackFrames({
            "assets/textures/pickups/weapons/item_10.png",
            "assets/textures/pickups/weapons/item_11.png",
            "assets/textures/pickups/weapons/item_12.png"
        });
    }
    else if (type == "pistol") {
        wep = std::make_unique<Pistol>(
            w.value("damage", 25.f),
            w.value("fireRate", 2.5f),
            w.value("projectileSpeed", 500.f),
            w.value("spread", 3.f),
            w.value("magazineSize", 12),
            w.value("reserveAmmo", 48),
            w.value("reloadTime", 1.2f),
            ammoTypeFromString(w.value("ammoType", std::string("small"))));
        wep->loadTexture("assets/textures/pickups/weapons/item_7.png");
    }
    else if (type == "shotgun") {
        wep = std::make_unique<Shotgun>(
            w.value("damage", 18.f),
            w.value("fireRate", 0.8f),
            w.value("projectileSpeed", 450.f),
            w.value("spread", 20.f),
            w.value("pelletsPerShot", 6),
            w.value("magazineSize", 6),
            w.value("reserveAmmo", 24),
            w.value("reloadTime", 2.0f),
            ammoTypeFromString(w.value("ammoType", std::string("medium"))));
        wep->loadTexture("assets/textures/pickups/weapons/item_4.png");
    }
    else if (type == "grenade") {
        wep = std::make_unique<Grenade>(
            w.value("damage", 120.f),
            w.value("fireRate", 1.0f),
            w.value("throwSpeed", 400.f),
            w.value("fuseTime", 2.0f),
            w.value("blastRadius", 100.f),
            w.value("magazineSize", 1),
            w.value("reserveAmmo", 3),
            w.value("reloadTime", 0.5f),
            ammoTypeFromString(w.value("ammoType", std::string("big"))));
        wep->loadTexture("assets/textures/pickups/weapons/item_3.png");
    }
    else {
        std::cerr << "[WeaponFactory] No factory case for weapon type: " << type << "\n";
        return nullptr;
    }

    return wep;
}

std::vector<std::unique_ptr<Weapon>> WeaponFactory::createDefaultLoadout() {
    std::vector<std::unique_ptr<Weapon>> loadout;
    loadout.push_back(create("knife"));
    loadout.push_back(create("pistol"));
    loadout.push_back(create("shotgun"));
    loadout.push_back(create("grenade"));
    return loadout;
}