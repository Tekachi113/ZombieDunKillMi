#include "TileMap.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cmath>

bool TileMap::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[TileMap] Cannot open: " << path << "\n";
        return false;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        std::cerr << "[TileMap] JSON parse error: " << e.what() << "\n";
        return false;
    }

    widthTiles  = j.value("width",    0);
    heightTiles = j.value("height",   0);
    tileSize    = j.value("tileSize", 16);
    std::string tilesetPath = j.value("tilesetPath", "");

    if (widthTiles <= 0 || heightTiles <= 0) {
        std::cerr << "[TileMap] Invalid map dimensions\n";
        return false;
    }

    // Load tileset texture
    if (!tileset.loadFromFile(tilesetPath)) {
        std::cerr << "[TileMap] Cannot load tileset: " << tilesetPath << "\n";
        return false;
    }
    tileset.setSmooth(false);
    tilesetColumns = static_cast<int>(tileset.getSize().x) / tileSize;

    // Load tile grid
    auto& rows = j.at("tiles");
    tileGrid.resize(heightTiles, std::vector<int>(widthTiles, 0));
    walkable.resize(heightTiles, std::vector<bool>(widthTiles, true));

    // Optional walkability layer
    bool hasWalkLayer = j.contains("walkable");

    for (int row = 0; row < heightTiles; ++row) {
        for (int col = 0; col < widthTiles; ++col) {
            int idx = rows[row][col].get<int>();
            tileGrid[row][col] = idx;
            // By convention: tile index 0 = empty/walkable, >0 = wall
            walkable[row][col] = (idx == 0);
        }
    }

    if (hasWalkLayer) {
        auto& wRows = j.at("walkable");
        for (int row = 0; row < heightTiles; ++row) {
            for (int col = 0; col < widthTiles; ++col) {
                walkable[row][col] = wRows[row][col].get<bool>();
            }
        }
    }

    buildVertices();
    std::cout << "[TileMap] Loaded " << widthTiles << "x" << heightTiles
              << " map from " << path << "\n";
    return true;
}

void TileMap::buildVertices() {
    vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    vertices.resize(static_cast<std::size_t>(widthTiles * heightTiles * 6));

    for (int row = 0; row < heightTiles; ++row) {
        for (int col = 0; col < widthTiles; ++col) {
            int tileIdx = tileGrid[row][col];
            if (tileIdx < 0) continue; // -1 = transparent/skip

            int tu = tileIdx % tilesetColumns;
            int tv = tileIdx / tilesetColumns;

            std::size_t base = static_cast<std::size_t>((row * widthTiles + col) * 6);

            float x = static_cast<float>(col * tileSize);
            float y = static_cast<float>(row * tileSize);
            float s = static_cast<float>(tileSize);

            float tx = static_cast<float>(tu * tileSize);
            float ty = static_cast<float>(tv * tileSize);

            // Triangle 1
            vertices[base + 0].position  = { x,     y     };
            vertices[base + 1].position  = { x + s, y     };
            vertices[base + 2].position  = { x,     y + s };
            // Triangle 2
            vertices[base + 3].position  = { x + s, y     };
            vertices[base + 4].position  = { x + s, y + s };
            vertices[base + 5].position  = { x,     y + s };

            // Texture coords
            vertices[base + 0].texCoords = { tx,     ty     };
            vertices[base + 1].texCoords = { tx + s, ty     };
            vertices[base + 2].texCoords = { tx,     ty + s };
            vertices[base + 3].texCoords = { tx + s, ty     };
            vertices[base + 4].texCoords = { tx + s, ty + s };
            vertices[base + 5].texCoords = { tx,     ty + s };
        }
    }
}

void TileMap::render(sf::RenderTarget& target, const sf::View& /*camera*/) const {
    sf::RenderStates states;
    states.texture = &tileset;
    target.draw(vertices, states);
}

bool TileMap::isWalkable(sf::Vector2f worldPos) const {
    int col = static_cast<int>(worldPos.x) / tileSize;
    int row = static_cast<int>(worldPos.y) / tileSize;
    if (col < 0 || row < 0 || col >= widthTiles || row >= heightTiles)
        return false;
    return walkable[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
}
