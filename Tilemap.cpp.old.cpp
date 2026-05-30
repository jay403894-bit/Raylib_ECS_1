#include "Tilemap.h"
// Function to asynchronously load the tilemap using TaskScheduler
void Tilemap::LoadTilemapAsync(const std::string& filePath) {
    TaskManager::Get()->AddTaskFunc([this, filePath]() {
        tson::Tileson parser;
        auto parsedMap = parser.parse(filePath);

        // After parsing, update the tilemap data and notify the main thread
        {
            std::lock_guard<std::mutex> lock(loadMutex);
            if (parsedMap->getStatus() == tson::ParseStatus::OK) {
                map = std::move(parsedMap);  // Move the parsed map into the member variable
            }
            else {
                std::cerr << "Failed to parse the tilemap!" << std::endl;
            }
            tilemapLoaded = true;  // Mark tilemap as loaded
        }

        // Notify the main thread to continue processing (loading texture and calculating solids)
        TaskManager::AddMainThreadTaskFunc([this]() {
            OnTilemapLoaded();  // Continue loading the texture and processing solids on the main thread
            });
        });
}

// Main thread callback to load the texture and process the tilemap
void Tilemap::OnTilemapLoaded() {
    if (!map) {
        std::cerr << "Tilemap is not loaded properly!" << std::endl;
        return;
    }

    // Load texture on the main thread
    const auto& tilesets = map->getTilesets();
    if (tilesets.empty()) return;

    const auto& ts = tilesets[0];
    tileWidth = ts.getTileSize().x;
    tileHeight = ts.getTileSize().y;
    columns = ts.getColumns();
    firstGid = ts.getFirstgid();

    texture = LoadTexture(ts.getImage().string().c_str());  // Load texture on the main thread
    if (texture.id == 0) {
        std::cerr << "Failed to load texture!" << std::endl;
        return;
    }

    // Process solid rectangles on the main thread
    solidRects.clear();
    for (auto& layer : map->getLayers()) {
        if (layer.getType() != tson::LayerType::TileLayer) continue;

        for (int y = 0; y < layer.getSize().y; ++y) {
            for (int x = 0; x < layer.getSize().x; ++x) {
                tson::Tile* tile = layer.getTileData(x, y);
                auto* prop = tile ? tile->getProperties().getProperty("isSolid") : nullptr;
                bool isSolid = prop && prop->getType() == tson::Type::Boolean && prop->getValue<bool>();

                if (isSolid) {
                    Rectangle rect = {
                        static_cast<float>(x * tileWidth),
                        static_cast<float>(y * tileHeight),
                        static_cast<float>(tileWidth),
                        static_cast<float>(tileHeight)
                    };
                    solidRects.push_back(rect);
                    std::cout << "Solid Tile at: " << rect.x << ", " << rect.y << std::endl;  // Debugging line
                }
            }
        }
    }
}

bool Tilemap::Load(const std::string& filePath) {
    tilemapLoaded = false;  // Reset the flag before starting loading
    LoadTilemapAsync(filePath);  // Start tilemap loading asynchronously

    // Wait for the tilemap to be loaded and processed on the main thread
    while (!tilemapLoaded) {
        // Optionally, you can add a delay or some form of polling here
        // For example, just let the main loop continue, or use a more sophisticated task-wait mechanism
    }

    return true;  // Indicate that the tilemap is loaded and ready to be drawn
}
tson::Layer* Tilemap::GetLayer(const std::string& layerName) {
    if (!map) return nullptr;

    // Iterate through layers to find the one with the given name
    for (auto& layer : map->getLayers()) {
        if (layer.getName() == layerName) {
            return &layer;  // Return the layer if the name matches
        }
    }

    return nullptr;  // Return nullptr if no matching layer is found
}
const std::vector<Rectangle>& Tilemap::GetSolidRects() const {
    return solidRects;
}

void Tilemap::Draw(Camera2D camera) const {
    if (!map) return;
    BeginMode2D(camera);
    // Loop through each layer and draw its tiles
    for (const auto& layer : map->getLayers()) {
        if (layer.getType() == tson::LayerType::TileLayer) {
            const auto& tileData = layer.getData();  // This gives you a vector of tile IDs

            // Loop through all tiles in the layer
            for (int y = 0; y < layer.getSize().y; ++y) {
                for (int x = 0; x < layer.getSize().x; ++x) {
                    // Get the tile ID from the data vector (considering x + y * width indexing)
                    unsigned int tileId = tileData[x + y * layer.getSize().x];

                    if (tileId > 0) {  // If tileId is valid (greater than 0 means it's a tile)
                        // Adjust tileIndex for 0-based index (subtract firstGid)
                        int tileIndex = tileId - firstGid;

                        // Get the texture's source rectangle based on the tile index
                        int tilesetColumns = map->getTilesets()[0].getColumns();
                        int tileX = tileIndex % tilesetColumns;
                        int tileY = tileIndex / tilesetColumns;

                        Rectangle sourceRect = {
                            tileX * tileWidth,
                            tileY * tileHeight,
                            static_cast<float>(tileWidth),
                            static_cast<float>(tileHeight)
                        };

                        // Calculate the position to draw the tile on the screen
                        Vector2 position = { static_cast<float>(x * tileWidth), static_cast<float>(y * tileHeight) };

                        // Draw the tile on the screen
                        DrawTextureRec(texture, sourceRect, position, WHITE);
                    }
                }
            }
        }
    }
}

const std::vector<tson::Layer>& Tilemap::getLayers() const {
    return map->getLayers();  // Tileson gives access to layers directly
}

Vector2 Tilemap::GetTileSize() {
    return { (float)tileWidth, (float)tileHeight };
}