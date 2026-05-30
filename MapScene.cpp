#include "Scenes.h"
#include "tileson.h"
#include <memory>
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "AnimationComponent.h"
#include "InputComponent.h"
MapScene::MapScene(const std::string& mapFilePath) {
    tMap.Load(mapFilePath, &world);

    auto layers = tMap.getLayers();
    std::cout << "Loaded map with " << layers.size() << " layers" << std::endl;
    if (!layers.empty()) {
        auto size = layers[0].getSize();
        std::cout << "Layer 0 size: " << size.x << ", " << size.y << std::endl;
    }

    Vector2 tileSize = tMap.GetTileSize();
    std::cout << "Tile size: " << tileSize.x << ", " << tileSize.y << std::endl;

    GetSpawnPoints();
    Spawn();
}

void MapScene::Update(bool& isRunning, float dt) {

}

void MapScene::HandleInput() {

    if (IsKeyPressed(KEY_ESCAPE)) {
        SceneManager::PopScene();
        // Possibly return to menu, trigger scene change, etc.
    }

}

void MapScene::Draw() {
    ClearBackground(SKYBLUE);

    if (!player) {
        std::cerr << "Error: Player object is not initialized!" << std::endl;
        return;
    }

    // Get player position
    Vector2 playerPos = player->GetComponent<TransformComponent>()->position;
   
    // Get map bounds
    float mapWidth = tMap.getLayers()[0].getSize().x * tMap.GetTileSize().x;
    float mapHeight = tMap.getLayers()[0].getSize().y * tMap.GetTileSize().y;
    
    if (mapWidth <= 0 || mapHeight <= 0) {
        std::cerr << "Error: Invalid map dimensions!" << std::endl;
        return;
    }

    // Set zoom
    camera.zoom = 2.0f;

    // Calculate visible area in world coordinates
    float visibleWidth = GetScreenWidth() / camera.zoom;
    float visibleHeight = GetScreenHeight() / camera.zoom;

    // Clamp target position to stay within map boundaries
    float minX = visibleWidth / 2;
    float maxX = mapWidth - visibleWidth / 2;
    float minY = visibleHeight / 2;
    float maxY = mapHeight - visibleHeight / 2;

    camera.target.x = std::clamp(playerPos.x, minX, maxX);
    camera.target.y = std::clamp(playerPos.y, minY, maxY);

    // Set offset to center camera on screen
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

    // Begin camera mode
    BeginMode2D(camera);

    // Draw map
    tMap.Draw(camera);
    // Update and draw playerqq
    player->Update(GetFrameTime());
    player->GetComponent<SpriteComponent>()->Draw();
    EndMode2D();
}


void MapScene::GetSpawnPoints() {
    std::vector<SpawnPoint> points;

    tson::Layer* objectLayer = tMap.GetLayer("Object Layer 1");
    if (objectLayer && objectLayer->getType() == tson::LayerType::ObjectGroup) {
        for (tson::Object& obj : objectLayer->getObjects()) {
            // Check if the object has a property named "entity"
            auto prop = obj.getProp("entity");
            if (prop) {
                SpawnPoint sp;
                sp.type = prop->getValue<std::string>();
                tson::Vector2i position = obj.getPosition();
                sp.position = { static_cast<float>(position.x), static_cast<float>(position.y) };
                points.push_back(sp);
            }
        }
    }

    spawnPoints = points;
}


void MapScene::Spawn() {
    player = std::make_unique<Entity>();
    Vector2 tileSize = tMap.GetTileSize();
    float playerWidth = 13.0f;
    float playerHeight = 19.0f;

    for (const auto& sp : spawnPoints) {
        if (sp.type == "player") {
            auto sprite = player->AddComponent<SpriteComponent>("Knight-Sheet.png", Rectangle{ 0, 0, playerWidth, playerHeight });
            player->AddComponent<TransformComponent>(sp.position);
            player->AddComponent<InputComponent>();
           //player->AddComponent<PhysicsComponent>(&world, 2.0f, 2.0f, 1.0f, 1.0f);

            // Spawn at bottom of tile, aligning feet with tile bottom
            float spawnX = sp.position.x;
            float spawnY = sp.position.y - playerHeight;

           
            // Initialize the camera's target to the player's position
            camera.target = { spawnX, spawnY };

            // Debug: Check if the camera target is correctly initialized
            std::cout << "Camera target initialized to: " << camera.target.x << ", " << camera.target.y << std::endl;
        }
    }
}
