#include "PlayState.h"
#include "core/Game.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>

// ---- Tileset paths relative to the executable (assets/ is copied next to it) ----
static const std::string TERRAIN_DIR =
    "assets/textures/terrain/variations/";
static const std::string WALL_PATH   =
    "assets/textures/terrain/wall/wall.png";
static const std::string PLAYER_WALK_DIR =
    "assets/textures/player/walk/";

// =========================================================
//  Constructor
// =========================================================
PlayState::PlayState(Game& game)
    : GameState(game)
    , player({mapPixelW() * 0.5f, mapPixelH() * 0.5f})   // spawn at map centre
    , camera(sf::FloatRect{{0.f, 0.f},
             {static_cast<float>(Game::WINDOW_WIDTH),
              static_cast<float>(Game::WINDOW_HEIGHT)}})
{
}

// =========================================================
//  onEnter
// =========================================================
void PlayState::onEnter() {
    std::cout << "[PlayState] Entering play state\n";

    // --- Load terrain textures ---
    terrainTiles.clear();
    // Load up to 4 terrain variation PNGs from the asset folder
    std::vector<std::string> terrainPaths;
    try {
        for (auto& e : std::filesystem::directory_iterator(TERRAIN_DIR)) {
            if (e.path().extension() == ".png")
                terrainPaths.push_back(e.path().string());
        }
    } catch (...) {}
    std::sort(terrainPaths.begin(), terrainPaths.end());
    terrainPaths.resize(std::min<int>(static_cast<int>(terrainPaths.size()), 4));

    for (auto& path : terrainPaths) {
        TileVariant tv;
        tv.loaded = tv.texture.loadFromFile(path);
        tv.texture.setSmooth(false);
        if (tv.loaded)
            std::cout << "[PlayState] Terrain tile: " << path << "\n";
        terrainTiles.push_back(std::move(tv));
    }

    // Fallback: solid colour tiles when no PNGs exist
    if (terrainTiles.empty()) {
        std::cout << "[PlayState] No terrain PNGs found — using colour tiles\n";
    }

    // --- Load wall tile ---
    wallTile.loaded = wallTile.texture.loadFromFile(WALL_PATH);
    wallTile.texture.setSmooth(false);

    // --- Load player walk animation ---
    player.loadAnimations(PLAYER_WALK_DIR);

    // --- Build the tile grid + vertex arrays ---
    buildMap();
    buildVertices();

    // --- HUD font ---
    if (hudFont.openFromFile("assets/fonts/default.ttf")) {
        pauseHint.emplace(hudFont);
        pauseHint->setString("WASD to move   ESC to pause");
        pauseHint->setCharacterSize(16);
        pauseHint->setFillColor(sf::Color(200, 200, 200, 180));
        pauseHint->setPosition({10.f, 10.f});
    }
}

void PlayState::onExit() {
    std::cout << "[PlayState] Exiting play state\n";
}

// =========================================================
//  buildMap — fill tileGrid with a simple layout
// =========================================================
void PlayState::buildMap() {
    std::srand(42); // fixed seed for reproducible map

    tileGrid.assign(MAP_ROWS, std::vector<int>(MAP_COLS, 0));

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            // Border = wall (tile index -1)
            if (r == 0 || r == MAP_ROWS - 1 || c == 0 || c == MAP_COLS - 1) {
                tileGrid[r][c] = -1; // wall
            } else {
                // Random ground variation (0..N-1)
                int variants = terrainTiles.empty() ? 1
                             : static_cast<int>(terrainTiles.size());
                // Weighted: 70% tile 0, rest random
                tileGrid[r][c] = (std::rand() % 10 < 7) ? 0
                                : (std::rand() % variants);
            }
        }
    }
}

// =========================================================
//  buildVertices — pre-build sf::VertexArray for fast render
// =========================================================
void PlayState::buildVertices() {
    // Count ground and wall quads separately
    int groundCount = 0, wallCount = 0;
    for (int r = 0; r < MAP_ROWS; ++r)
        for (int c = 0; c < MAP_COLS; ++c)
            (tileGrid[r][c] == -1 ? wallCount : groundCount)++;

    groundVerts.setPrimitiveType(sf::PrimitiveType::Triangles);
    groundVerts.resize(static_cast<std::size_t>(groundCount * 6));

    wallVerts.setPrimitiveType(sf::PrimitiveType::Triangles);
    wallVerts.resize(static_cast<std::size_t>(wallCount * 6));

    // Helper: fill 2 triangles at base index
    auto fillQuad = [&](sf::VertexArray& va, std::size_t base,
                        float x, float y, float s,
                        float tx, float ty, float ts,
                        sf::Color tint) {
        // Tri 1
        va[base+0] = {{x,   y  }, tint, {tx,    ty   }};
        va[base+1] = {{x+s, y  }, tint, {tx+ts, ty   }};
        va[base+2] = {{x,   y+s}, tint, {tx,    ty+ts}};
        // Tri 2
        va[base+3] = {{x+s, y  }, tint, {tx+ts, ty   }};
        va[base+4] = {{x+s, y+s}, tint, {tx+ts, ty+ts}};
        va[base+5] = {{x,   y+s}, tint, {tx,    ty+ts}};
    };

    std::size_t gi = 0, wi = 0;
    const float S = static_cast<float>(TILE_PX);

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            float px = static_cast<float>(c * TILE_PX);
            float py = static_cast<float>(r * TILE_PX);
            int   idx = tileGrid[r][c];

            if (idx == -1) {
                // Wall tile — single texture (or fallback colour)
                float ts = wallTile.loaded
                           ? static_cast<float>(wallTile.texture.getSize().x)
                           : S;
                sf::Color col = wallTile.loaded ? sf::Color::White
                                                : sf::Color(60, 40, 20);
                fillQuad(wallVerts, wi * 6, px, py, S, 0.f, 0.f, ts, col);
                ++wi;
            } else {
                // Ground tile — use variation texture or solid colour
                bool hasTex = (idx < static_cast<int>(terrainTiles.size()))
                              && terrainTiles[idx].loaded;
                float ts = hasTex
                    ? static_cast<float>(terrainTiles[idx].texture.getSize().x)
                    : S;

                // Slight random brightness variation for visual interest
                int bv = 200 + (std::rand() % 30);
                sf::Color col = hasTex ? sf::Color::White
                                       : sf::Color(
                                           static_cast<std::uint8_t>(bv - 40),
                                           static_cast<std::uint8_t>(bv),
                                           static_cast<std::uint8_t>(bv - 60));
                fillQuad(groundVerts, gi * 6, px, py, S, 0.f, 0.f, ts, col);
                ++gi;
            }
        }
    }
}

// =========================================================
//  handleEvent
// =========================================================
void PlayState::handleEvent(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            // TODO: push PauseState
            std::cout << "[PlayState] ESC — pause (not yet implemented)\n";
        }
    }
}

// =========================================================
//  update
// =========================================================
void PlayState::update(float dt) {
    // Mouse world position (convert screen coords through the camera view)
    game.getWindow().setView(camera);
    sf::Vector2f mouseWorld = game.getWindow().mapPixelToCoords(
        sf::Mouse::getPosition(game.getWindow()));
    game.getWindow().setView(game.getWindow().getDefaultView());

    // Let the player face the cursor
    player.setAimTarget(mouseWorld);

    // Input-driven movement
    player.handleInput(game.getInput(), dt);

    // Clamp player inside the walkable area (1 tile border = wall)
    const float margin = static_cast<float>(TILE_PX) + Player::RADIUS;
    sf::Vector2f pos = player.getPosition();
    pos.x = std::max(margin, std::min(pos.x, mapPixelW() - margin));
    pos.y = std::max(margin, std::min(pos.y, mapPixelH() - margin));
    player.setPosition(pos);

    player.update(dt);

    // Follow camera
    camera.setCenter(pos);
    clampCamera();
}

void PlayState::clampCamera() {
    float halfW = camera.getSize().x * 0.5f;
    float halfH = camera.getSize().y * 0.5f;

    sf::Vector2f c = camera.getCenter();
    c.x = std::max(halfW, std::min(c.x, mapPixelW() - halfW));
    c.y = std::max(halfH, std::min(c.y, mapPixelH() - halfH));
    camera.setCenter(c);
}

// =========================================================
//  render
// =========================================================
void PlayState::render(sf::RenderTarget& target) {
    target.setView(camera);

    // Draw ground
    if (!terrainTiles.empty()) {
        // Group tiles by texture and draw in batches
        // Simple approach: draw each tile type separately
        for (std::size_t ti = 0; ti < terrainTiles.size(); ++ti) {
            sf::RenderStates rs;
            rs.texture = terrainTiles[ti].loaded ? &terrainTiles[ti].texture : nullptr;
            // Build a per-variant vertex array on the fly (small map, OK for now)
            // For a full game this would be pre-built per variant
            sf::VertexArray va(sf::PrimitiveType::Triangles);
            for (int r = 0; r < MAP_ROWS; ++r) {
                for (int c = 0; c < MAP_COLS; ++c) {
                    if (tileGrid[r][c] != static_cast<int>(ti)) continue;
                    float px = static_cast<float>(c * TILE_PX);
                    float py = static_cast<float>(r * TILE_PX);
                    float S  = static_cast<float>(TILE_PX);
                    float ts = terrainTiles[ti].loaded
                               ? static_cast<float>(terrainTiles[ti].texture.getSize().x)
                               : S;
                    sf::Color col = terrainTiles[ti].loaded
                                  ? sf::Color::White
                                  : sf::Color(80, 120, 60);
                    va.append({{px,   py  }, col, {0.f, 0.f}});
                    va.append({{px+S, py  }, col, {ts,  0.f}});
                    va.append({{px,   py+S}, col, {0.f, ts }});
                    va.append({{px+S, py  }, col, {ts,  0.f}});
                    va.append({{px+S, py+S}, col, {ts,  ts }});
                    va.append({{px,   py+S}, col, {0.f, ts }});
                }
            }
            target.draw(va, rs);
        }
    } else {
        // Fallback: no textures — draw solid green ground
        sf::RectangleShape ground({mapPixelW(), mapPixelH()});
        ground.setFillColor(sf::Color(60, 100, 50));
        target.draw(ground);
    }

    // Draw walls
    {
        sf::RenderStates rs;
        rs.texture = wallTile.loaded ? &wallTile.texture : nullptr;
        sf::VertexArray va(sf::PrimitiveType::Triangles);
        for (int r = 0; r < MAP_ROWS; ++r) {
            for (int c = 0; c < MAP_COLS; ++c) {
                if (tileGrid[r][c] != -1) continue;
                float px = static_cast<float>(c * TILE_PX);
                float py = static_cast<float>(r * TILE_PX);
                float S  = static_cast<float>(TILE_PX);
                float ts = wallTile.loaded
                           ? static_cast<float>(wallTile.texture.getSize().x)
                           : S;
                sf::Color col = wallTile.loaded ? sf::Color::White
                                               : sf::Color(50, 35, 20);
                va.append({{px,   py  }, col, {0.f, 0.f}});
                va.append({{px+S, py  }, col, {ts,  0.f}});
                va.append({{px,   py+S}, col, {0.f, ts }});
                va.append({{px+S, py  }, col, {ts,  0.f}});
                va.append({{px+S, py+S}, col, {ts,  ts }});
                va.append({{px,   py+S}, col, {0.f, ts }});
            }
        }
        target.draw(va, rs);
    }

    // Draw player
    player.render(target);

    // HUD — switch to default view so it's screen-space
    target.setView(target.getDefaultView());
    if (pauseHint) target.draw(*pauseHint);

    // Health bar
    {
        float barW  = 200.f, barH = 16.f, barX = 10.f, barY = 34.f;
        sf::RectangleShape bgBar({barW, barH});
        bgBar.setPosition({barX, barY});
        bgBar.setFillColor(sf::Color(60, 0, 0));
        target.draw(bgBar);

        float pct = player.getHealth() / player.getMaxHealth();
        sf::RectangleShape hpBar({barW * pct, barH});
        hpBar.setPosition({barX, barY});
        hpBar.setFillColor(sf::Color(220, 40, 40));
        target.draw(hpBar);
    }
}
