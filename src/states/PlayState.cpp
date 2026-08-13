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

    std::random_device rdDev;
    std::mt19937 g(rdDev());
    auto& rm2 = game.getResources();

    // Helpers — place at explicit tile (col, row) centre
    auto placeAt = [&](int col, int row, const std::string& key, bool collidable) {
        if (!rm2.hasTexture(key)) return;
        if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) return;
        sf::Vector2f pos{(col + 0.5f) * TILE_PX, (row + 0.5f) * TILE_PX};
        entityManager.add(std::make_unique<SceneryObject>(pos, rm2.getTexture(key), collidable));
    };
    auto placeVar = [&](int col, int row, const std::string& prefix, int maxV, bool collidable) {
        std::vector<int> ok;
        for (int v = 0; v < maxV; ++v)
            if (rm2.hasTexture(prefix + std::to_string(v))) ok.push_back(v);
        if (ok.empty()) return;
        std::uniform_int_distribution<int> pick(0, (int)ok.size() - 1);
        sf::Vector2f pos{(col + 0.5f) * TILE_PX, (row + 0.5f) * TILE_PX};
        entityManager.add(std::make_unique<SceneryObject>(pos, rm2.getTexture(prefix + std::to_string(ok[pick(g)])), collidable));
    };
    auto placeAnim = [&](int col, int row, const std::string& dir, float speed, bool collidable) {
        auto frames = loadFramesFromDir(dir);
        if (frames.empty()) return;
        sf::Vector2f pos{(col + 0.5f) * TILE_PX, (row + 0.5f) * TILE_PX};
        entityManager.add(std::make_unique<AnimatedScenery>(pos, std::move(frames), speed, collidable));
    };

    // ================================================================
    //  ZONE 1 — FARM (top-left, cols 1-19, rows 1-12)
    // ================================================================
    // Barn (large structure, top-centre of farm)
    placeVar(7,  2, "barn_",    3, true);
    placeVar(7,  5, "barn_",    3, true);
    // Tractor parked near barn
    placeVar(3,  5, "tractor_", 2, true);
    // Windmill (animated) in farm yard
    placeAnim(13, 7, "assets/textures/animated/windmill", 0.10f, true);
    // Crop field (dense grid, rows 3-6, cols 10-18)
    for (int r = 3; r <= 6; ++r)
        for (int c = 10; c <= 17; ++c)
            if ((r + c) % 2 == 0) placeVar(c, r, "crop_", 4, false);
    // Grass patches in farm open area
    for (int r = 7; r <= 11; ++r)
        for (int c = 2; c <= 9; c += 2)
            placeVar(c, r, "grass_", 4, false);
    // Straw bales next to barn
    placeVar(10, 2, "straw_", 2, false);
    placeVar(11, 2, "straw_", 2, false);
    // Fence rows along top and right of farm
    for (int c = 1; c <= 18; ++c) placeVar(c,  1, "fence_", 3, true);
    for (int r = 2; r <= 11; ++r) placeVar(18, r, "fence_", 3, true);
    // Scarecrow in the crop field
    placeVar(14, 4, "scarecrow_", 2, false);
    // Trees lining the left edge of the farm
    for (int r = 2; r <= 11; r += 2) placeVar(1, r, "tree_b_", 6, true);
    // Birds near windmill
    placeAnim(15, 6, "assets/textures/animated/bird_black", 0.08f, false);

    // ================================================================
    //  ZONE 2 — TOWN (top-right, cols 21-38, rows 1-12)
    // ================================================================
    // Gas station (top-left of town zone)
    placeVar(22, 2, "gasstation_", 2, true);
    placeVar(22, 5, "gasstation_", 2, true);
    // Small buildings scattered
    placeVar(27, 2, "bldg_sm_",   3, true);
    placeVar(32, 2, "bldg_sm_",   3, true);
    placeVar(36, 2, "bldg_bg_",   3, true);
    placeVar(27, 6, "bldg_sm_",   3, true);
    placeVar(33, 6, "bldg_bg_",   3, true);
    // Urban clutter (fences, barrels, urban objects)
    for (int c = 21; c <= 38; ++c) placeVar(c, 1, "fence_", 3, true);
    placeVar(25,  4, "urban_", 6, true);
    placeVar(26,  4, "urban_", 6, true);
    placeVar(30,  4, "urban_", 6, true);
    placeVar(31,  4, "urban_", 6, true);
    placeVar(35,  8, "urban_", 6, true);
    placeVar(36,  8, "urban_", 6, true);
    // Exploding barrels near gas station
    if (rm2.hasTexture("barrel")) {
        entityManager.add(std::make_unique<ExplodingBarrel>(sf::Vector2f{25.5f * TILE_PX, 3.5f * TILE_PX}, rm2.getTexture("barrel")));
        entityManager.add(std::make_unique<ExplodingBarrel>(sf::Vector2f{26.5f * TILE_PX, 3.5f * TILE_PX}, rm2.getTexture("barrel")));
    }
    // Trees lining town streets
    for (int r = 2; r <= 11; r += 3) placeVar(38, r, "tree_b_", 6, true);
    for (int c = 22; c <= 38; c += 4) placeVar(c, 10, "tree_b_", 6, true);
    // Crates behind buildings
    if (rm2.hasTexture("crate")) {
        for (int c = 23; c <= 26; ++c)
            entityManager.add(std::make_unique<BreakableBox>(sf::Vector2f{(c + 0.5f) * TILE_PX, 8.5f * TILE_PX}, rm2.getTexture("crate")));
    }
    // Road path markings on vertical divider (col 20)
    for (int r = 2; r <= 11; ++r) placeVar(20, r, "path_", 6, false);

    // ================================================================
    //  ZONE 3 — ROAD (rows 13-15, full width)
    // ================================================================
    // Road tiles as non-collidable decoration on top of road terrain
    for (int c = 2; c <= 38; c += 3) placeVar(c, 14, "road_", 6, false);
    // Crosswalk areas: cols 8-10 and cols 28-30
    for (int r = 13; r <= 15; ++r) {
        placeVar( 9, r, "path_", 6, false);
        placeVar(29, r, "path_", 6, false);
    }
    // Broken car on the road
    placeVar(16, 14, "car_", 6, true);
    placeVar(24, 14, "car_", 6, true);
    // Truck blocking road
    placeVar(33, 14, "truck_", 2, true);
    // Tire / debris
    placeVar(18, 14, "car_", 6, false);

    // ================================================================
    //  ZONE 4 — OPEN CENTRE (rows 16-21, cols 2-37 — player spawns here)
    // ================================================================
    // Sparse decoration so player has room to fight
    placeVar( 5, 18, "tree_b_",    6, true);
    placeVar(10, 19, "bush_b_",    6, true);
    placeVar(15, 17, "tree_b_",    6, true);
    placeVar(25, 18, "bush_b_",    6, true);
    placeVar(30, 20, "tree_b_",    6, true);
    placeVar(35, 17, "bush_b_",    6, true);
    placeVar( 8, 20, "blood_stain_",5, false);
    placeVar(22, 21, "blood_stain_",5, false);
    placeAt (18, 19, "zombie_poster", false);
    // A few crates for cover
    if (rm2.hasTexture("crate")) {
        for (int c : {6, 13, 20, 28, 34}) {
            entityManager.add(std::make_unique<BreakableBox>(sf::Vector2f{(c + 0.5f) * TILE_PX, 21.5f * TILE_PX}, rm2.getTexture("crate")));
        }
    }
    if (rm2.hasTexture("barrel")) {
        entityManager.add(std::make_unique<ExplodingBarrel>(sf::Vector2f{7.5f * TILE_PX, 21.5f * TILE_PX}, rm2.getTexture("barrel")));
        entityManager.add(std::make_unique<ExplodingBarrel>(sf::Vector2f{32.5f * TILE_PX, 18.5f * TILE_PX}, rm2.getTexture("barrel")));
    }

    // ================================================================
    //  ZONE 5 — FOREST EDGE (rows 22-28, full width)
    // ================================================================
    // Dense tree line
    for (int r = 22; r <= 27; ++r)
        for (int c = 1; c <= 38; c += 2)
            placeVar(c, r, "tree_b_", 6, true);
    // Bush undergrowth between trees
    for (int r = 23; r <= 26; r += 2)
        for (int c = 2; c <= 37; c += 3)
            placeVar(c, r, "bush_b_", 6, true);
    // Grass beneath the trees
    for (int c = 2; c <= 37; c += 2)
        placeVar(c, 28, "grass_", 4, false);
    // Water pond in the forest
    placeAnim(10, 25, "assets/textures/animated/water", 0.20f, false);
    placeAnim(11, 25, "assets/textures/animated/water", 0.20f, false);
    placeAnim(30, 24, "assets/textures/animated/water", 0.20f, false);
    // Dead bird near the pond
    placeAnim(12, 26, "assets/textures/animated/bird_black", 0.08f, false);

    // ================================================================
    //  ZONE 6 — GRAVEYARD (bottom-left corner, cols 1-10, rows 22-27)
    // ================================================================
    // Fence border
    for (int c = 1; c <= 9; ++c) placeVar(c, 22, "fence_", 3, true);
    for (int r = 23; r <= 27; ++r) placeVar(1, r, "fence_", 3, true);
    // Tombstones
    for (int r = 23; r <= 26; ++r)
        for (int c = 2; c <= 8; c += 2)
            placeAt(c, r, "tombstone", false);
    // Blood stains
    for (int c = 3; c <= 7; c += 2)
        placeVar(c, 25, "blood_stain_", 5, false);

    // ================================================================
    //  ZONE 7 — GAS STATION ROAD JUNCTION (below town, cols 21-38 rows 16-21)
    // ================================================================
    placeVar(24, 17, "urban_", 6, true);
    placeVar(24, 18, "urban_", 6, true);
    placeVar(35, 20, "urban_", 6, true);
    if (rm2.hasTexture("coke_can")) {
        for (int c = 22; c <= 26; ++c)
            entityManager.add(std::make_unique<BreakableBox>(sf::Vector2f{(c + 0.5f) * TILE_PX, 16.5f * TILE_PX}, rm2.getTexture("coke_can")));
    }
    // Bridge
    placeVar(38, 19, "bridge_", 3, true);
    // Music notes near (atmosphere)
    placeAnim(36, 17, "assets/textures/animated/music_notes", 0.25f, false);

    std::cout << "[PlayState] Spawned " << entityManager.countTotal() << " world entities\n";

    hud.load();
}



void PlayState::onExit() {
    std::cout << "[PlayState] Exiting play state\n";
    entityManager.clear();
}


void PlayState::buildMap() {
    // Deterministic seed so the map looks the same each run
    std::srand(42);

    // Start everything as floor tile variant 0
    tileGrid.assign(MAP_ROWS, std::vector<int>(MAP_COLS, 0));

    int variants = terrainTiles.empty() ? 1 : static_cast<int>(terrainTiles.size());

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
            // Border = wall
            if (r == 0 || r == MAP_ROWS - 1 || c == 0 || c == MAP_COLS - 1) {
                tileGrid[r][c] = -1;
                continue;
            }

            // --- Road strip: rows 13-15 (horizontal road through middle) ---
            if (r >= 13 && r <= 15) {
                tileGrid[r][c] = 1;   // darker variant = road look
                continue;
            }

            // --- Path tiles: light paths connecting zones ---
            // Vertical path col 20 (farm-to-town divider)
            if (c == 20 && r >= 1 && r <= 12) {
                tileGrid[r][c] = 2;
                continue;
            }
            // Vertical path col 20 below road
            if (c == 20 && r >= 16 && r <= MAP_ROWS - 2) {
                tileGrid[r][c] = 2;
                continue;
            }

            // --- Terrain variation: mostly variant 0, occasional 1-3 ---
            tileGrid[r][c] = (std::rand() % 8 < 6) ? 0 : (1 + std::rand() % (variants - 1 > 0 ? variants - 1 : 1));
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
            game.getStateManager().changeState(
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
            std::make_unique<GameOverState>(game));
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