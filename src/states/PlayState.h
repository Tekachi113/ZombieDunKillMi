#pragma once
#include "world/SpawnManager.h"
#include "ui/HUD.h"
#include "core/StateManager.h"
#include "entities/Player.h"
#include "world/EntityManager.h"
#include "world/CollisionSystem.h"
#include "world/TileMap.h"
#include "entities/pickups/AmmoSpawner.h"
#include <SFML/Graphics.hpp>
#include <vector>

class PlayState : public GameState {
public:
    explicit PlayState(Game& game);

    void handleEvent(const sf::Event& event) override;
    void update(float dt)                   override;
    void render(sf::RenderTarget& target)   override;
    void onEnter()                          override;
    void onExit()                           override;

private:
    // ---- Player ----
    Player player;

    // ---- Entities ----
    EntityManager   entityManager;
    CollisionSystem collisionSystem;
    SpawnManager    spawnManager;
    AmmoSpawner     ammoSpawner;
    // ---- Camera ----
    sf::View camera;

    // ---- Procedural tile map ----
    TileMap                  tileMap; // backing map for collision queries
    // We render a simple tiled ground using the terrain sprites.
    struct TileVariant {
        sf::Texture texture;
        bool loaded = false;
    };

    static constexpr int TILE_PX   = 48;    
    static constexpr int MAP_COLS  = 40;
    static constexpr int MAP_ROWS  = 30;

    
    std::vector<std::vector<int>> tileGrid;

    // Loaded terrain textures
    std::vector<TileVariant> terrainTiles; 
    TileVariant              wallTile;     

    sf::VertexArray groundVerts;
    sf::VertexArray wallVerts;

    
	HUD hud;

    // ---- Helpers ----
    void buildMap();
    void buildVertices();
    void clampCamera();
    bool initialized = false;
    float mapPixelW() const { return static_cast<float>(MAP_COLS * TILE_PX); }
    float mapPixelH() const { return static_cast<float>(MAP_ROWS * TILE_PX); }
};
