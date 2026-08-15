#include "UpgradeShopState.h"
#include "GameOverState.h"
#include "PauseState.h"
#include "PlayState.h"
#include "core/Game.h"
#include "entities/world_objects/WorldObjects.h"
#include "world/TileMap.h"
#include "entities/zombies/Zombie.h"
#include "world/SpawnManager.h"
#include "entities/weapons/WeaponFactory.h"
#include "entities/weapons/Weapon.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>
#include <random>
#include <string>
#include "entities/pickups/AmmoSpawner.h"

// Helper: load all PNGs from a directory as a texture vector (sorted by filename)
static std::vector<sf::Texture> loadFramesFromDir(const std::string& dir) {
    std::vector<sf::Texture> frames;
    std::vector<std::string> paths;
    try {
        for (auto& e : std::filesystem::directory_iterator(dir))
            if (e.path().extension() == ".png")
                paths.push_back(e.path().string());
    } catch (...) {}
    std::sort(paths.begin(), paths.end());
    frames.reserve(paths.size());
    for (auto& p : paths) {
        sf::Texture t;
        if (t.loadFromFile(p)) {
            t.setSmooth(false);
            frames.push_back(std::move(t));
        }
    }
    return frames;
}

// Helper: try to load a texture; silently skip if the file is missing
static void tryLoadTexture(class ResourceManager& rm,
                            const std::string& id,
                            const std::string& path) {
    try { rm.loadTexture(id, path); }
    catch (...) { /* file missing — skip silently */ }
}


static const std::string TERRAIN_DIR =
    "assets/textures/terrain/variations/";
static const std::string WALL_PATH   =
    "assets/textures/terrain/wall/wall.png";
static const std::string PLAYER_WALK_DIR =
    "assets/textures/player/walk/";


PlayState::PlayState(Game& game)
    : GameState(game)
    , player({mapPixelW() * 0.5f, mapPixelH() * 0.5f})   // spawn at map centre
    , camera(sf::FloatRect{{0.f, 0.f},
             {static_cast<float>(Game::WINDOW_WIDTH),
              static_cast<float>(Game::WINDOW_HEIGHT)}})
{
}


void PlayState::onEnter() {
    std::cout << "[PlayState] Entering play state\n";
	if (initialized) {
        return;
        }
    initialized = true;


    terrainTiles.clear();
 
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

    if (terrainTiles.empty()) {
        std::cout << "[PlayState] No terrain PNGs found — using colour tiles\n";
    }

    wallTile.loaded = wallTile.texture.loadFromFile(WALL_PATH);
    wallTile.texture.setSmooth(false);

    // ---- Preload static textures into ResourceManager ---
    // Use tryLoadTexture so missing files are skipped without crashing.
    auto& rm = game.getResources();

    // Legacy objects (crates / barrels)
    tryLoadTexture(rm, "crate",  "assets/textures/objects/crate.png");
    tryLoadTexture(rm, "barrel", "assets/textures/objects/barrel.png");

    // Ground decorations (non-collidable)
    for (int i = 0; i < 4; ++i)
        tryLoadTexture(rm, "grass_"        + std::to_string(i), "assets/textures/scenery/grass_"        + std::to_string(i) + ".png");
    for (int i = 0; i < 4; ++i)
        tryLoadTexture(rm, "crop_"         + std::to_string(i), "assets/textures/scenery/crop_"         + std::to_string(i) + ".png");
    for (int i = 0; i < 2; ++i)
        tryLoadTexture(rm, "straw_"        + std::to_string(i), "assets/textures/scenery/straw_"        + std::to_string(i) + ".png");
    for (int i = 0; i < 2; ++i)
        tryLoadTexture(rm, "scarecrow_"    + std::to_string(i), "assets/textures/scenery/scarecrow_"    + std::to_string(i) + ".png");
    tryLoadTexture(rm, "tombstone",     "assets/textures/scenery/tombstone_0.png");
    tryLoadTexture(rm, "zombie_poster", "assets/textures/scenery/zombie_poster_0.png");
    for (int i = 0; i < 5; ++i)
        tryLoadTexture(rm, "blood_stain_"  + std::to_string(i), "assets/textures/scenery/blood_stain_"  + std::to_string(i) + ".png");
    for (int i = 0; i < 6; ++i)
        tryLoadTexture(rm, "path_"         + std::to_string(i), "assets/textures/scenery/path_"         + std::to_string(i) + ".png");

    // Collidable structures
    for (int i = 0; i < 3; ++i)
        tryLoadTexture(rm, "fence_"        + std::to_string(i), "assets/textures/buildings/fence_"        + std::to_string(i) + ".png");
    for (int i = 0; i < 3; ++i)
        tryLoadTexture(rm, "bldg_sm_"      + std::to_string(i), "assets/textures/buildings/building_small_" + std::to_string(i) + ".png");
    for (int i = 0; i < 3; ++i)
        tryLoadTexture(rm, "bldg_bg_"      + std::to_string(i), "assets/textures/buildings/building_big_"   + std::to_string(i) + ".png");
    for (int i = 0; i < 3; ++i)
        tryLoadTexture(rm, "barn_"         + std::to_string(i), "assets/textures/buildings/barn_"           + std::to_string(i) + ".png");
    for (int i = 0; i < 2; ++i)
        tryLoadTexture(rm, "gasstation_"   + std::to_string(i), "assets/textures/buildings/gas_station_"    + std::to_string(i) + ".png");
    for (int i = 0; i < 6; ++i)
        tryLoadTexture(rm, "urban_"        + std::to_string(i), "assets/textures/buildings/urban_"          + std::to_string(i) + ".png");
    for (int i = 0; i < 6; ++i)
        tryLoadTexture(rm, "bush_b_"       + std::to_string(i), "assets/textures/buildings/bush_"           + std::to_string(i) + ".png");
    for (int i = 0; i < 6; ++i)
        tryLoadTexture(rm, "tree_b_"       + std::to_string(i), "assets/textures/buildings/tree_"           + std::to_string(i) + ".png");
    for (int i = 0; i < 6; ++i)
        tryLoadTexture(rm, "road_"         + std::to_string(i), "assets/textures/buildings/road_"           + std::to_string(i) + ".png");

    // Vehicles
    for (int i = 0; i < 6; ++i)
        tryLoadTexture(rm, "car_"          + std::to_string(i), "assets/textures/vehicles/car_"          + std::to_string(i) + ".png");
    for (int i = 0; i < 2; ++i)
        tryLoadTexture(rm, "truck_"        + std::to_string(i), "assets/textures/vehicles/truck_"        + std::to_string(i) + ".png");
    for (int i = 0; i < 2; ++i)
        tryLoadTexture(rm, "tractor_"      + std::to_string(i), "assets/textures/vehicles/tractor_"      + std::to_string(i) + ".png");
    for (int i = 0; i < 3; ++i)
        tryLoadTexture(rm, "bridge_"       + std::to_string(i), "assets/textures/vehicles/bridge_"       + std::to_string(i) + ".png");

    // Breakable coke can
    tryLoadTexture(rm, "coke_can", "assets/textures/breakable/coke_can_0.png");

    player.loadAnimations(PLAYER_WALK_DIR);
    player.setWeapons(WeaponFactory::createDefaultLoadout());

    buildMap();
    buildVertices();

    // Initialize TileMap for collision
    tileMap.initialize(MAP_COLS, MAP_ROWS, TILE_PX);
    for (int r = 0; r < MAP_ROWS; ++r)
        for (int c = 0; c < MAP_COLS; ++c)
            tileMap.setTile(c, r, tileGrid[r][c], (tileGrid[r][c] != -1));
    spawnManager.loadWaveConfig("assets/data/wave_config.json");

    spawnManager.startWave(
        0,
        sf::FloatRect(
            { 0.f, 0.f },
            { mapPixelW(), mapPixelH() }
        )
    );

    Zombie::setTarget(&player);
    Zombie::setEntityManager(&entityManager);
    BreakableBox::setEntityManager(&entityManager);
    BreakableBox::setPlayer(&player);

    auto& rm2 = game.getResources();

    // ---- Simple scattered crates and barrels for cover ----
    // Build a list of valid floor tile positions (away from the player spawn)
    std::vector<sf::Vector2f> spawnPoints;
    sf::Vector2f playerPos = player.getPosition();
    for (int r = 2; r < MAP_ROWS - 2; ++r) {
        for (int c = 2; c < MAP_COLS - 2; ++c) {
            if (tileGrid[r][c] == -1) continue;
            float px = (c + 0.5f) * TILE_PX;
            float py = (r + 0.5f) * TILE_PX;
            float dx = px - playerPos.x, dy = py - playerPos.y;
            // Keep a clear circle around the player spawn
            if (dx*dx + dy*dy < (TILE_PX * 4.f) * (TILE_PX * 4.f)) continue;
            spawnPoints.push_back({px, py});
        }
    }
    std::mt19937 g(42u); // fixed seed for reproducible layout
    std::shuffle(spawnPoints.begin(), spawnPoints.end(), g);

    std::size_t idx = 0;

    // Crates
    if (rm2.hasTexture("crate")) {
        for (int i = 0; i < 30 && idx < spawnPoints.size(); ++i, ++idx)
            entityManager.add(std::make_unique<BreakableBox>(spawnPoints[idx], rm2.getTexture("crate")));
    }

    // Exploding barrels
    if (rm2.hasTexture("barrel")) {
        for (int i = 0; i < 10 && idx < spawnPoints.size(); ++i, ++idx)
            entityManager.add(std::make_unique<ExplodingBarrel>(spawnPoints[idx], rm2.getTexture("barrel")));
    }

    std::cout << "[PlayState] Spawned " << entityManager.countTotal() << " world entities\n";

    hud.load();
}






void PlayState::onExit() {
    std::cout << "[PlayState] Exiting play state\n";
}


void PlayState::buildMap() {
    std::srand(42);

    // Flat ground everywhere; only the border is a wall
    tileGrid.assign(MAP_ROWS, std::vector<int>(MAP_COLS, 0));

    int variants = terrainTiles.empty() ? 1 : static_cast<int>(terrainTiles.size());

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            if (r == 0 || r == MAP_ROWS - 1 || c == 0 || c == MAP_COLS - 1) {
                tileGrid[r][c] = -1; // border wall
            } else {
                // Light terrain variation
                tileGrid[r][c] = (std::rand() % 10 < 7) ? 0
                               : (std::rand() % variants);
            }
        }
    }
}



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

void PlayState::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            game.getStateManager().pushState(
                std::make_unique<PauseState>(game));
        }

        if (key->code == sf::Keyboard::Key::U)
        {
            game.getStateManager().pushState(
                std::make_unique<UpgradeShopState>(game, player));
        }
    }
}

void PlayState::update(float dt) {
    // Mouse world position (convert screen coords through the camera view)
    game.getWindow().setView(camera);
    sf::Vector2f mouseWorld = game.getWindow().mapPixelToCoords(
        sf::Mouse::getPosition(game.getWindow()));
    game.getWindow().setView(game.getWindow().getDefaultView());

    
    player.setAimTarget(mouseWorld);

    
    player.handleInput(game.getInput(), dt);
    player.handleCombat(game.getInput(), entityManager);

    const float margin = static_cast<float>(TILE_PX) + Player::RADIUS;
    sf::Vector2f pos = player.getPosition();
    pos.x = std::max(margin, std::min(pos.x, mapPixelW() - margin));
    pos.y = std::max(margin, std::min(pos.y, mapPixelH() - margin));
    player.setPosition(pos);

    player.update(dt);
    hud.update(player);
    spawnManager.update(dt, entityManager, player);
    ammoSpawner.update(dt,entityManager,
        sf::FloatRect(
            { 0.f, 0.f },
            { mapPixelW(), mapPixelH() }
        ),
        &tileMap
    );
    if (!player.isAlive())
    {
        game.getStateManager().changeState(
            std::make_unique<GameOverState>(game, player.getScore()));
        return;
    }
    entityManager.update(dt);

    // Resolve all collisions (Player & other entities vs tiles, obstacles, etc.)
    collisionSystem.resolve(entityManager, tileMap, player);

    // Remove dead entities (e.g. broken crates, exploded barrels)
    entityManager.removeDead();

    // Follow camera
    camera.setCenter(player.getPosition()); 
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


    if (!terrainTiles.empty()) {

        for (std::size_t ti = 0; ti < terrainTiles.size(); ++ti) {
            sf::RenderStates rs;
            rs.texture = terrainTiles[ti].loaded ? &terrainTiles[ti].texture : nullptr;

            sf::VertexArray va(sf::PrimitiveType::Triangles);
            for (int r = 0; r < MAP_ROWS; ++r) {
                for (int c = 0; c < MAP_COLS; ++c) {
                    if (tileGrid[r][c] != static_cast<int>(ti)) continue;
                    float px = static_cast<float>(c * TILE_PX);
                    float py = static_cast<float>(r * TILE_PX);
                    float S = static_cast<float>(TILE_PX);
                    float ts = terrainTiles[ti].loaded
                        ? static_cast<float>(terrainTiles[ti].texture.getSize().x)
                        : S;
                    sf::Color col = terrainTiles[ti].loaded
                        ? sf::Color::White
                        : sf::Color(80, 120, 60);
                    va.append({ {px,   py  }, col, {0.f, 0.f} });
                    va.append({ {px + S, py  }, col, {ts,  0.f} });
                    va.append({ {px,   py + S}, col, {0.f, ts } });
                    va.append({ {px + S, py  }, col, {ts,  0.f} });
                    va.append({ {px + S, py + S}, col, {ts,  ts } });
                    va.append({ {px,   py + S}, col, {0.f, ts } });
                }
            }
            target.draw(va, rs);
        }
    }
    else {
        // Fallback: no textures — draw solid green ground
        sf::RectangleShape ground({ mapPixelW(), mapPixelH() });
        ground.setFillColor(sf::Color(60, 100, 50));
        target.draw(ground);
    }


    {
        sf::RenderStates rs;
        rs.texture = wallTile.loaded ? &wallTile.texture : nullptr;
        sf::VertexArray va(sf::PrimitiveType::Triangles);
        for (int r = 0; r < MAP_ROWS; ++r) {
            for (int c = 0; c < MAP_COLS; ++c) {
                if (tileGrid[r][c] != -1) continue;
                float px = static_cast<float>(c * TILE_PX);
                float py = static_cast<float>(r * TILE_PX);
                float S = static_cast<float>(TILE_PX);
                float ts = wallTile.loaded
                    ? static_cast<float>(wallTile.texture.getSize().x)
                    : S;
                sf::Color col = wallTile.loaded ? sf::Color::White
                    : sf::Color(50, 35, 20);
                va.append({ {px,   py  }, col, {0.f, 0.f} });
                va.append({ {px + S, py  }, col, {ts,  0.f} });
                va.append({ {px,   py + S}, col, {0.f, ts } });
                va.append({ {px + S, py  }, col, {ts,  0.f} });
                va.append({ {px + S, py + S}, col, {ts,  ts } });
                va.append({ {px,   py + S}, col, {0.f, ts } });
            }
        }
        target.draw(va, rs);
    }

    // Draw world entities (crates, barrels, trees, bushes)
    entityManager.render(target);

    // Draw player
    player.render(target);


    hud.render(target);
}