#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

// Forward declarations
class Entity;
class EntityManager;
class Player;

// =========================================================
//  WaveDefinition — data for one wave loaded from JSON
// =========================================================
struct ZombieSpawnEntry {
    std::string type;   // "small" | "medium" | "big" | "turret"
    int         count;
};

struct WaveDefinition {
    int waveNumber;
    float spawnInterval;          // seconds between each zombie spawn
    std::vector<ZombieSpawnEntry> zombies;
};

// =========================================================
//  SpawnManager — controls wave progression and zombie spawning
// =========================================================
class SpawnManager {
public:
    // Load wave definitions from assets/data/wave_config.json
    bool loadWaveConfig(const std::string& path);

    // Call once to start a wave by index
    void startWave(int waveIndex, const sf::FloatRect& mapBounds);

    // Drive spawning every frame
    void update(float dt, EntityManager& entities, const Player& player);

    // Query state
    bool isWaveComplete()  const;  // true when all zombies spawned AND dead
    bool isWaveInProgress() const { return waveActive; }
    int  getCurrentWave()   const { return currentWaveIndex; }
    int  getTotalWaves()    const { return static_cast<int>(waveDefs.size()); }

private:
    std::vector<WaveDefinition> waveDefs;

    int   currentWaveIndex      = 0;
    bool  waveActive            = false;
    float spawnTimer            = 0.f;
    int   zombiesLeftToSpawn    = 0;
    int   zombieSpawnCursor     = 0; // index into current wave's zombie list
    sf::FloatRect spawnBounds;

    // Spawn one zombie at a random off-screen edge position
    void spawnNext(EntityManager& entities, const Player& player);
    sf::Vector2f randomSpawnPos(const Player& player) const;
};
