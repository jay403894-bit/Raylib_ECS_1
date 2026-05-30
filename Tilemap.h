#pragma once
#include "raylib.h"
#include "Tilesheet.h"
#include "EntityManager.h"
#include "Components.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
/*
# Map metadata
width: 10
height: 8
tileSize: 32

# Tiles layer (one line per row)
# Format per tile:  tilesheetIndex,id,solid,destructible,restitution,friction
tiles:
0,1,1,0,0.2,0.5  0,1,1,0,0.2,0.5  0,0,0,0,0,0  ... (width tiles)
...
*/
struct MapTile {
    int tilesheetIndex;  // Which tilesheet
    Tile tile; //the tile
    PhysicsMaterial2D material; // Optional per-tile physics

};

class Tilemap {
private:
    int width, height;
    int tileSize;
    std::vector<MapTile> tiles;
    std::vector<Tilesheet> tilesheets;  // vector of all loaded tilesheets

public:
    // Default constructor
    Tilemap() : width(0), height(0), tileSize(0), tiles() {}

    // Constructor with size
    Tilemap(int w, int h, int tSize)
        : width(w), height(h), tileSize(tSize), tiles(w* h) {
    }


    void AddTilesheet(const Tilesheet& sheet) {
        tilesheets.push_back(sheet);
    }

    MapTile& GetTile(int x, int y) { return tiles[y * width + x]; }

    void Draw() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                MapTile& mt = GetTile(x, y);
                if (mt.tile.id < 0) continue;

                const Tilesheet& sheet = tilesheets[mt.tilesheetIndex];
                const Tile& t = sheet.GetTile(mt.tile.id);

                DrawTextureRec(sheet.GetTexture(), t.srcRect,
                    { float(x * tileSize), float(y * tileSize) }, WHITE);
            }
        }
    }
    /*
    void CreateMergedColliders(std::shared_ptr<EntityManager>& entityManager)
    {
        std::cout << "CreateMergedColliders CALLED\n";

        for (int y = 0; y < height; ++y) {
            int runStart = -1;

            for (int x = 0; x <= width; ++x) {
                bool isSolid = false;

                if (x < width) {
                    const MapTile& tile = tiles[y * width + x];
                    isSolid = tile.material.solid;
                }

                if (isSolid) {
                    if (runStart == -1) {
                        runStart = x;
                    }
                }
                else if (runStart != -1) {
                    int runLength = x - runStart;

                    float worldX = runStart * tileSize;
                    float worldY = y * tileSize;
                    float widthPixels = runLength * tileSize;

                    // Create the entity
                    auto colliderEntity = std::make_shared<Entity>();
                    Vector2 topLeft = { worldX, worldY };

                    colliderEntity->AddComponent<TransformComponent>(topLeft);
                    auto collider = colliderEntity->AddComponent<BoxCollider2D>(
                        Rectangle{ topLeft.x, topLeft.y, widthPixels, (float)tileSize }
                    );

                    collider->debugDraw = true;
                    collider->isStatic = true;
                    colliderEntity->AddComponent<TagComponent>("MergedTile");
                    std::cout << "Adding collider at (" << worldX << ", " << worldY
                        << ") size (" << widthPixels << ", " << tileSize << ")\n";
                    entityManager->AddEntity(colliderEntity);

                    runStart = -1;
                }
            }
        }
    }
    */
    void CreateMergedColliders(std::shared_ptr<EntityManager>& entityManager)
    {
        for (int y = 0; y < height; ++y) {
            int runStart = -1;
            PhysicsMaterial2D currentMat{}; // Tracks material of the run

            for (int x = 0; x <= width; ++x) {

                bool isSolid = false;
                PhysicsMaterial2D tileMat{};

                if (x < width) {
                    const MapTile& tile = tiles[y * width + x];
                    isSolid = tile.material.solid;
                    tileMat = tile.material;
                }

                // Check if we can continue the current run:
                bool sameRun = false;
                if (runStart != -1) {
                    // Compare material to the one from the start of the run
                    sameRun =
                        (tileMat.solid == currentMat.solid) &&
                        (tileMat.restitution == currentMat.restitution) &&
                        (tileMat.friction == currentMat.friction) &&
                        (tileMat.destructible == currentMat.destructible);
                }

                // Start a new run if none is active
                if (isSolid && runStart == -1) {
                    runStart = x;
                    currentMat = tileMat;
                }
                // Continue run only if same material
                else if (isSolid && !sameRun) {
                    // Finalize the previous run
                    int runLength = x - runStart;
                    float worldX = runStart * tileSize;
                    float worldY = y * tileSize;
                    float widthPixels = runLength * tileSize;

                    auto colliderEntity = std::make_shared<Entity>();
                    Vector2 topLeft = { worldX, worldY };
                    colliderEntity->AddComponent<TransformComponent>(topLeft);

                    auto collider = colliderEntity->AddComponent<BoxCollider2D>(
                        Rectangle{ topLeft.x, topLeft.y, widthPixels, (float)tileSize }
                    );
                    collider->debugDraw = true;
                    collider->isStatic = true;
                    colliderEntity->AddComponent<TagComponent>("MergedTile");
                    colliderEntity->AddComponent<RigidBody2D>();
                    auto rb = colliderEntity->GetComponent<RigidBody2D>();
                    rb->useGravity = false;
                    rb->material = currentMat;
                    entityManager->AddEntity(colliderEntity);

                    // Start a new run with the new tile
                    runStart = x;
                    currentMat = tileMat;
                }
                // End run if current tile isn't solid
                else if (!isSolid && runStart != -1) {
                    // Finish the run
                    int runLength = x - runStart;
                    float worldX = runStart * tileSize;
                    float worldY = y * tileSize;
                    float widthPixels = runLength * tileSize;

                    auto colliderEntity = std::make_shared<Entity>();
                    Vector2 topLeft = { worldX, worldY };
                    colliderEntity->AddComponent<TransformComponent>(topLeft);

                    auto collider = colliderEntity->AddComponent<BoxCollider2D>(
                        Rectangle{ topLeft.x, topLeft.y, widthPixels, (float)tileSize }
                    );
                    collider->debugDraw = true;
                    collider->isStatic = true;

                    colliderEntity->AddComponent<TagComponent>("MergedTile");
                    entityManager->AddEntity(colliderEntity);

                    runStart = -1;
                }
            }
        }
    }

    bool LoadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open tilemap file: " << path << std::endl;
            return false;
        }

        std::string line;
        int y = 0;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (line.find("width:") == 0) {
                width = std::stoi(line.substr(6));
            }
            else if (line.find("height:") == 0) {
                height = std::stoi(line.substr(7));
            }
            else if (line.find("tileSize:") == 0) {
                tileSize = std::stoi(line.substr(9));
            }
            else if (line.find("tiles:") == 0) {
                tiles.resize(width * height);
                y = 0;
            }
            else {
                std::istringstream rowStream(line);
                std::string tileData;
                int x = 0;

                while (rowStream >> tileData) {
                    MapTile t;
                    std::replace(tileData.begin(), tileData.end(), ',', ' ');
                    std::istringstream tileStream(tileData);
                    tileStream >> t.tilesheetIndex >> t.tile.id
                        >> t.material.solid
                        >> t.material.destructible
                        >> t.material.restitution
                        >> t.material.friction;

                    // Ensure the tilesheet index is valid
                    if (t.tilesheetIndex < 0 || t.tilesheetIndex >= tilesheets.size()) {
                        std::cerr << "Invalid tilesheet index: " << t.tilesheetIndex << std::endl;
                        continue;
                    }

                    const Tilesheet& sheet = tilesheets[t.tilesheetIndex];
                    Texture2D sheetTex = sheet.GetTexture();
                    int sheetTileSize = sheet.GetTileSize(); // or however you store it
                    int tilesPerRow = sheetTex.width / sheetTileSize;

                    // Compute the srcRect based on the tile's ID and the tilesheet
                    t.tile.srcRect.x = float((t.tile.id % tilesPerRow) * sheetTileSize);
                    t.tile.srcRect.y = float((t.tile.id / tilesPerRow) * sheetTileSize);
                    t.tile.srcRect.width = float(sheetTileSize);
                    t.tile.srcRect.height = float(sheetTileSize);

                    tiles[y * width + x] = t;
                    x++;
                }
                y++;
            }
        }

        return true;
    }

    void SetTile(int x, int y, const MapTile& tile) {
        tiles[y * width + x] = tile;
    }

    Rectangle GetTileWorldBounds(int x, int y) {
        return Rectangle{ float(x * tileSize), float(y * tileSize), float(tileSize), float(tileSize) };
    }

};



/*
# Map metadata
width: 10
height: 8
tileSize: 32

# Tiles layer (one line per row)
# Format per tile:  id,solid,destructible,restitution,friction
tiles:
1,1,0,0.2,0.5  1,1,0,0.2,0.5  0,0,0,0,0  ... (width tiles)
...

class TileMap {
private:
    int width, height;                    // in tiles
    int tileSize;                         // size of each tile

    std::vector<Tile> tiles;              // 1D array for simplicity

public:
    TileMap(int w, int h, int tSize)
        : width(w), height(h), tileSize(tSize), tiles(w* h) {
    }
    bool LoadFromFile(const std::string& path, Texture2D tileset) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open tilemap file: " << path << std::endl;
            return false;
        }

        std::string line;
        int y = 0;

        // Calculate how many tiles fit per row in the tileset
        int tilesPerRow = tileset.width / tileSize;

        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;

            if (line.find("width:") == 0) width = std::stoi(line.substr(6));
            else if (line.find("height:") == 0) height = std::stoi(line.substr(7));
            else if (line.find("tileSize:") == 0) tileSize = std::stoi(line.substr(9));
            else if (line.find("tiles:") == 0) {
                tiles.resize(width * height); // allocate tile array
                y = 0; // reset row counter
            }
            else {
                std::istringstream rowStream(line);
                std::string tileData;
                int x = 0;
                while (rowStream >> tileData) {
                    Tile t;
                    std::replace(tileData.begin(), tileData.end(), ',', ' ');
                    std::istringstream tileStream(tileData);
                    tileStream >> t.id >> t.material.solid >> t.material.destructible >> t.material.restitution >> t.material.friction;

                    // Calculate srcRect based on ID
                    t.srcRect.x = float((t.id % tilesPerRow) * tileSize);
                    t.srcRect.y = float((t.id / tilesPerRow) * tileSize);
                    t.srcRect.width = float(tileSize);
                    t.srcRect.height = float(tileSize);

                    tiles[y * width + x] = t;
                    x++;
                }
                y++;
            }
        }

        return true;
    }
    Tile& GetTile(int x, int y) {
        return tiles[y * width + x];
    }

    void SetTile(int x, int y, const Tile& tile) {
        tiles[y * width + x] = tile;
    }

    Rectangle GetTileWorldBounds(int x, int y) {
        return Rectangle{ float(x * tileSize), float(y * tileSize), float(tileSize), float(tileSize) };
    }

    void Draw(Texture2D tileset) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Tile& t = GetTile(x, y);
                if (t.id < 0) continue; // empty
                DrawTextureRec(tileset, t.srcRect, { float(x * tileSize), float(y * tileSize) }, WHITE);
            }
        }
    }
};*/