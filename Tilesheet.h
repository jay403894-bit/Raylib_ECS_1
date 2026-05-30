#pragma once
#include "Tile.h"
#include "raylib.h"
#include "PhysicsMaterial.h"
#include <vector>

struct Tile {
    int id;                   // Tile type or sprite ID
    Rectangle srcRect;         // Sprite UV
};

class Tilesheet {
private:
    int tileSize;
    int tilesPerRow, tilesPerColumn;
    Texture2D texture;
    std::vector<Tile> tiles; // Only holds tile metadata, id, physics etc.

public:
    Tilesheet() = default;

    bool Load(const std::string& path, int tSize) {
        texture = LoadTexture(path.c_str());
        if (texture.id == 0) return false;

        tileSize = tSize;
        tilesPerRow = texture.width / tileSize;
        tilesPerColumn = texture.height / tileSize;

        // Initialize tiles vector
        int totalTiles = tilesPerRow * tilesPerColumn;
        tiles.resize(totalTiles);

        for (int i = 0; i < totalTiles; ++i) {
            tiles[i].id = i;
            tiles[i].srcRect = {
                float((i % tilesPerRow) * tileSize),
                float((i / tilesPerRow) * tileSize),
                float(tileSize),
                float(tileSize)
            };

        }

        return true;
    }

    const Tile& GetTile(int id) const { return tiles[id]; }

    Texture2D GetTexture() const { return texture; }
    int GetTileSize() const { return tileSize; }
    void Draw(Vector2 position, int spacing = 2) const
    {
        if (texture.id == 0) return;

        int xOffset = 0;
        int yOffset = 0;

        for (const Tile& t : tiles)
        {
            Rectangle destRect = {
                position.x + float(xOffset),
                position.y + float(yOffset),
                float(tileSize),
                float(tileSize)
            };

            DrawTextureRec(texture, t.srcRect, { destRect.x, destRect.y }, WHITE);

            xOffset += tileSize + spacing;
            if (xOffset + tileSize > 512) // wrap after 512 pixels width for preview
            {
                xOffset = 0;
                yOffset += tileSize + spacing;
            }
        }
    }
};
