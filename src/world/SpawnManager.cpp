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
        def.waveNumber = wj.value("wave", 0);
        def.spawnInterval = wj.value("spawnInterval", 1.5f);
        for (auto& ej : wj.at("zombies")) {
            ZombieSpawnEntry entry;
            entry.type = ej.value("type", "small");
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
    currentWaveIndex = waveIndex;
    spawnBounds = mapBounds;
    waveActive = true;
    spawnTimer = 0.f;
    zombieSpawnCursor = 0;

    zombiesLeftToSpawn = 0;
    for (auto& e : waveDefs[waveIndex].zombies)
        zombiesLeftToSpawn += e.count;

    std::cout << "[SpawnManager] Wave " << (waveIndex + 1)
        << " started - " << zombiesLeftToSpawn << " zombies\n";
}

void SpawnManager::update(float dt, EntityManager& entities, const Player& player) {
    if (!waveActive) return;

    if (zombiesLeftToSpawn > 0) {
        spawnTimer += dt;
        float interval = waveDefs[currentWaveIndex].spawnInterval;

        if (spawnTimer >= interval) {
            spawnTimer -= interval;
            spawnNext(entities, player);
        }
        return; // still spawning this wave, nothing else to do yet
    }

    // Every zombie for this wave has been spawned. Wait until they're
    // all dead, then auto-advance to the next (harder) wave -- this is
    // what makes zombies "keep coming more and more" instead of the
    // game going silent forever after wave 1. Once the last wave in
    // wave_config.json is reached, it loops back to the last wave again
    // (repeats forever, staying at max difficulty) instead of stopping.
    int aliveZombies = 0;
    for (Entity* e : entities.getAllOf<Zombie>()) {
        if (e->isAlive()) ++aliveZombies;
    }

    if (aliveZombies == 0) {
        int next = currentWaveIndex + 1;
        if (next >= static_cast<int>(waveDefs.size())) {
            next = static_cast<int>(waveDefs.size()) - 1; // stay on hardest wave
        }
        startWave(next, spawnBounds);
    }
}

bool SpawnManager::isWaveComplete() const {
    return waveActive && zombiesLeftToSpawn <= 0;
}

void SpawnManager::spawnNext(EntityManager& entities, const Player& player) {
    if (zombiesLeftToSpawn <= 0) return;

    auto& zombieList = waveDefs[currentWaveIndex].zombies;
    if (zombieList.empty()) { --zombiesLeftToSpawn; return; }

    // zombieSpawnCursor counts how many zombies have been spawned so far
    // THIS wave. Walk the list to find which entry that index falls
    // into, e.g. [{small,5},{medium,3}] -> spawn # 0-4 = small, 5-7 =
    // medium. (Previous version compared the running sum against
    // zombieList.size() instead of the actual spawn index, so it almost
    // always picked the first type in the list and ignored the rest.)
    std::string type = zombieList.back().type;
    int counted = 0;
    for (auto& entry : zombieList) {
        counted += entry.count;
        if (zombieSpawnCursor < counted) {
            type = entry.type;
            break;
        }
    }
    ++zombieSpawnCursor;

    sf::Vector2f pos = randomSpawnPos(player);

    if (type == "small") {
        entities.add(std::make_unique<SmallZombie>(pos));
    }
    else if (type == "medium") {
        entities.add(std::make_unique<MediumZombie>(pos));
    }
    else if (type == "big") {
        entities.add(std::make_unique<BigZombie>(pos));
    }
    else if (type == "turret") {
        entities.add(std::make_unique<TurretZombie>(pos));
    }

    --zombiesLeftToSpawn;
    std::cout << "[SpawnManager] Spawned " << type
        << " - " << zombiesLeftToSpawn << " remaining\n";
}

sf::Vector2f SpawnManager::randomSpawnPos(const Player& /*player*/) const {
    // IMPORTANT: spawn just INSIDE the map edge, not outside it.
    // CollisionSystem::resolveTileCollisions() runs on every entity,
    // including zombies, and TileMap::isWalkable() returns false for
    // any position outside the grid. A zombie spawned past the edge
    // gets permanently pushed back against the boundary by collision
    // resolution and can never walk onto the actual map -- it just
    // gets stuck off-screen forever. Keeping spawns inside spawnBounds
    // avoids that trap while still spawning at the edges (far from
    // wherever the player currently is).
    const float margin = 40.f; // inset from the edge, not outset

    float rw = spawnBounds.size.x;
    float rh = spawnBounds.size.y;
    float rx = spawnBounds.position.x;
    float ry = spawnBounds.position.y;

    int edge = std::rand() % 4;
    sf::Vector2f pos;
    switch (edge) {
    case 0: pos = { rx + static_cast<float>(std::rand() % static_cast<int>(rw)), ry + margin }; break;          // top
    case 1: pos = { rx + static_cast<float>(std::rand() % static_cast<int>(rw)), ry + rh - margin }; break;     // bottom
    case 2: pos = { rx + margin, ry + static_cast<float>(std::rand() % static_cast<int>(rh)) }; break;          // left
    case 3: pos = { rx + rw - margin, ry + static_cast<float>(std::rand() % static_cast<int>(rh)) }; break;     // right
    }

    return pos;
}