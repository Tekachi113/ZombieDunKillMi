#include "SpawnManager.h"
#include "EntityManager.h"
#include "../entities/Entity.h"
#include "../entities/zombies/Zombie.h"
#include "../entities/Player.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <nlohmann/json.hpp>

bool SpawnManager::loadWaveConfig(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[SpawnManager] Cannot open wave config: " << path << "\n";
        return false;
    }

    nlohmann::json j;
    try { file >> j; }
    catch (const std::exception& e) {
        std::cerr << "[SpawnManager] JSON error: " << e.what() << "\n";
        return false;
    }

    waveDefs.clear();
    for (auto& wj : j.at("waves")) {
        WaveDefinition def;
        def.waveNumber    = wj.value("wave",          0);
        def.spawnInterval = wj.value("spawnInterval", 1.5f);
        for (auto& ej : wj.at("zombies")) {
            ZombieSpawnEntry entry;
            entry.type  = ej.value("type",  "small");
            entry.count = ej.value("count", 1);
            def.zombies.push_back(entry);
        }
        waveDefs.push_back(def);
    }

    std::cout << "[SpawnManager] Loaded " << waveDefs.size() << " waves\n";
    return true;
}

void SpawnManager::startWave(int waveIndex, const sf::FloatRect& mapBounds) {
    if (waveIndex < 0 || waveIndex >= static_cast<int>(waveDefs.size())) {
        std::cerr << "[SpawnManager] Wave index out of range: " << waveIndex << "\n";
        return;
    }
    currentWaveIndex   = waveIndex;
    spawnBounds        = mapBounds;
    waveActive         = true;
    spawnTimer         = 0.f;
    zombieSpawnCursor  = 0;

    // Count total zombies this wave
    zombiesLeftToSpawn = 0;
    for (auto& e : waveDefs[waveIndex].zombies)
        zombiesLeftToSpawn += e.count;

    std::cout << "[SpawnManager] Wave " << (waveIndex + 1)
              << " started — " << zombiesLeftToSpawn << " zombies\n";
}

void SpawnManager::update(float dt, EntityManager& entities, const Player& player) {
    if (!waveActive || zombiesLeftToSpawn <= 0) return;

    spawnTimer += dt;
    float interval = waveDefs[currentWaveIndex].spawnInterval;

    if (spawnTimer >= interval) {
        spawnTimer -= interval;
        spawnNext(entities, player);
    }
}

bool SpawnManager::isWaveComplete() const {
    // Wave is complete when all zombies have been spawned AND none are alive
    // (EntityManager tracking is the responsibility of PlayState)
    return waveActive && zombiesLeftToSpawn <= 0;
}

void SpawnManager::spawnNext(EntityManager& entities, const Player& player) {
    if (zombiesLeftToSpawn <= 0) return;

    // Find which zombie type to spawn next using the cursor
    auto& zombieList = waveDefs[currentWaveIndex].zombies;
    int cursor = 0;
    std::string type = zombieList.empty() ? "small" : zombieList.back().type;
    for (auto& entry : zombieList) {
        cursor += entry.count;
        type = entry.type;
        if (cursor >= static_cast<int>(zombieList.size())) break;
    }

    sf::Vector2f pos = randomSpawnPos(player);

    if (type == "small") {
        entities.add(std::make_unique<SmallZombie>(pos));
    } else if (type == "medium") {
        entities.add(std::make_unique<MediumZombie>(pos));
    } else if (type == "big") {
        entities.add(std::make_unique<BigZombie>(pos));
    } else if (type == "turret") {
        entities.add(std::make_unique<TurretZombie>(pos));
    }

    --zombiesLeftToSpawn;
    std::cout << "[SpawnManager] Spawned " << type
              << " — " << zombiesLeftToSpawn << " remaining\n";
}

sf::Vector2f SpawnManager::randomSpawnPos(const Player& /*player*/) const {
    // Spawn off-screen on a random edge
    const float margin = 80.f;

    sf::Vector2f pos;
    int attempts = 0;
    do {
        int edge = std::rand() % 4;
        float rw = spawnBounds.size.x;
        float rh = spawnBounds.size.y;
        float rx = spawnBounds.position.x;
        float ry = spawnBounds.position.y;

        switch (edge) {
            case 0: pos = { rx + static_cast<float>(std::rand() % static_cast<int>(rw)), ry - margin }; break; // top
            case 1: pos = { rx + static_cast<float>(std::rand() % static_cast<int>(rw)), ry + rh + margin }; break; // bottom
            case 2: pos = { rx - margin, ry + static_cast<float>(std::rand() % static_cast<int>(rh)) }; break; // left
            case 3: pos = { rx + rw + margin, ry + static_cast<float>(std::rand() % static_cast<int>(rh)) }; break; // right
        }
        ++attempts;
    } while (attempts < 10 /* enough tries */);

    return pos;
}
