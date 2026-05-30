#include "Tilemap.h"
#include "Entity.h"
#include "Components.h"
// Main thread callback to load the texture and process the tilemap
bool Tilemap::Load(const std::string& filePath, std::shared_ptr<EntityManager>& entityManager) {
    tson::Tileson parser;
    auto parsedMap = parser.parse(filePath);

    if (parsedMap->getStatus() != tson::ParseStatus::OK) {
        std::cerr << "Failed to parse the tilemap!" << std::endl;
        return false;
    }

    map = std::move(parsedMap);

    const auto& tilesets = map->getTilesets();
    if (tilesets.empty()) {
        std::cerr << "No tilesets found in the map!" << std::endl;
        return false;
    }

    const auto& ts = tilesets[0];
    tileWidth = ts.getTileSize().x;
    tileHeight = ts.getTileSize().y;
    columns = ts.getColumns();
    firstGid = ts.getFirstgid();

    texture = LoadTexture(ts.getImage().string().c_str());
    if (texture.id == 0) {
        std::cerr << "Failed to load texture!" << std::endl;
        return false;
    }

    // For each tile layer
    for (auto& layer : map->getLayers()) {
        if (layer.getType() != tson::LayerType::TileLayer) continue;

        int width = layer.getSize().x;
        int height = layer.getSize().y;
        const auto& tileData = layer.getData();

        // 1) Create tile entities with sprites
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                unsigned int gid = tileData[x + y * width];
                if (gid == 0) continue; // skip empty tile

                int tileIndex = gid - firstGid;
                int tileX = tileIndex % columns;
                int tileY = tileIndex / columns;

                Rectangle srcRect = {
                    (float)(tileX * tileWidth),
                    (float)(tileY * tileHeight),
                    (float)tileWidth,
                    (float)tileHeight
                };

                Vector2 worldPos = {
                    (float)(x * tileWidth),
                    (float)(y * tileHeight)
                };

                auto tileEntity = std::make_shared<Entity>();
                tileEntity->AddComponent<TransformComponent>(worldPos);
                tileEntity->AddComponent<SpriteComponent>(texture, srcRect);

                entityManager->AddEntity(tileEntity);
            }
        }

        // 2) Create merged colliders ONCE per layer
        for (int y = 0; y < height; ++y) {
            int runStart = -1;

            for (int x = 0; x <= width; ++x) {
                tson::Tile* tile = (x < width) ? layer.getTileData(x, y) : nullptr;

                bool isSolid = false;
                if (tile) {
                    auto props = tile->getProperties();
                    if (props.hasProperty("isSolid")) {
                        auto* prop = props.getProperty("isSolid");
                        if (prop && prop->getType() == tson::Type::Boolean) {
                            isSolid = prop->getValue<bool>();
                        }
                    }
                }

                if (isSolid) {
                    if (runStart == -1) runStart = x;
                }
                else if (runStart != -1) {
                    int runLength = x - runStart;
                    float worldX = runStart * tileWidth;
                    float worldY = y * tileHeight;
                    float widthPixels = runLength * tileWidth;

                    auto colliderEntity = std::make_shared<Entity>();
                    Vector2 topLeft = { worldX, worldY };  // no offset!
                    colliderEntity->AddComponent<TransformComponent>(topLeft);
                   auto collider = colliderEntity->AddComponent<BoxCollider2D>(
                        Rectangle{ topLeft.x, topLeft.y, widthPixels, (float)tileHeight }
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

    return true;
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

const std::vector<tson::Layer>& Tilemap::getLayers() const {
    return map->getLayers();  // Tileson gives access to layers directly
}

Vector2 Tilemap::GetTileSize() {
    return { (float)tileWidth, (float)tileHeight };
}