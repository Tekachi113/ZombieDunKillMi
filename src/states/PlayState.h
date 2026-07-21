#pragma once

#include "core/StateManager.h"
#include "entities/Player.h"
#include <SFML/Graphics.hpp>
#include <vector>

// =========================================================
//  PlayState — main gameplay: map + player movement
// =========================================================
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

    // ---- Camera ----
    sf::View camera;

    // ---- Procedural tile map ----
    // We render a simple tiled ground using the terrain sprites.
    // Full JSON-driven TileMap comes once we wire it in.
    struct TileVariant {
        sf::Texture texture;
        bool loaded = false;
    };

    static constexpr int TILE_PX   = 48;    // 16px * 3 scale
    static constexpr int MAP_COLS  = 40;
    static constexpr int MAP_ROWS  = 30;

    // Grid of tile indices (0-3 for the 4 terrain variations)
    std::vector<std::vector<int>> tileGrid;

    // Loaded terrain textures
    std::vector<TileVariant> terrainTiles; // ground
    TileVariant              wallTile;     // border wall

    sf::VertexArray groundVerts;
    sf::VertexArray wallVerts;

    // ---- HUD font (borrowed from MenuState pattern) ----
    sf::Font                 hudFont;
    std::optional<sf::Text>  pauseHint;

    // ---- Helpers ----
    void buildMap();
    void buildVertices();
    void clampCamera();

    float mapPixelW() const { return static_cast<float>(MAP_COLS * TILE_PX); }
    float mapPixelH() const { return static_cast<float>(MAP_ROWS * TILE_PX); }
};
