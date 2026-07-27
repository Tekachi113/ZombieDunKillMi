#include "PauseState.h"
#include "PlayState.h"
#include "core/Game.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>

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

    
    player.loadAnimations(PLAYER_WALK_DIR);

    buildMap();
    buildVertices();

    
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


void PlayState::buildMap() {
    std::srand(42); 

    tileGrid.assign(MAP_ROWS, std::vector<int>(MAP_COLS, 0));

    for (int r = 0; r < MAP_ROWS; ++r) {
        for (int c = 0; c < MAP_COLS; ++c) {
          
            if (r == 0 || r == MAP_ROWS - 1 || c == 0 || c == MAP_COLS - 1) {
                tileGrid[r][c] = -1; 
            } else {
               
                int variants = terrainTiles.empty() ? 1
                             : static_cast<int>(terrainTiles.size());
               
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

    
    player.render(target);

    
    target.setView(target.getDefaultView());
    if (pauseHint) target.draw(*pauseHint);

    
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
