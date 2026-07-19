#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

// =========================================================
//  TileMap — loads, stores and renders the tile-based world
// =========================================================
class TileMap {
public:
    // Load map from a JSON file.
    // Expected format: { "width": N, "height": M, "tileSize": 16,
    //                    "tilesetPath": "...", "tiles": [[...], ...] }
    bool loadFromFile(const std::string& path);

    // Render only the tiles visible in the current camera view
    void render(sf::RenderTarget& target, const sf::View& camera) const;

    // Returns true if the tile at worldPos is walkable (not a wall/obstacle)
    bool isWalkable(sf::Vector2f worldPos) const;

    // Map dimensions in tiles / pixels
    int getWidthTiles()  const { return widthTiles; }
    int getHeightTiles() const { return heightTiles; }
    int getTileSize()    const { return tileSize; }
    sf::Vector2f getSizePixels() const {
        return { static_cast<float>(widthTiles  * tileSize),
                 static_cast<float>(heightTiles * tileSize) };
    }

private:
    // Tile layer: tileGrid[row][col] = tile index (0 = empty/walkable)
    std::vector<std::vector<int>> tileGrid;

    // Walkability layer: true = walkable
    std::vector<std::vector<bool>> walkable;

    sf::Texture   tileset;
    sf::VertexArray vertices;  // pre-built quad array for fast rendering

    int widthTiles  = 0;
    int heightTiles = 0;
    int tileSize    = 16;
    int tilesetColumns = 1; // how many tile columns in the tileset image

    void buildVertices();    // called after loading to pre-build vertex array
};
