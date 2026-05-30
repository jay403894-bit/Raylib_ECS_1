#include "SceneManager.h"
std::stack <std::unique_ptr<Scene>> SceneManager::scenes;

void SceneManager::PushScene(std::unique_ptr<Scene> newScene) {
    scenes.push(std::move(newScene));
}

void SceneManager::PopScene() {
    if (!scenes.empty()) {
        scenes.pop(); // remove the current scene
    }
}

void SceneManager::ReplaceScene(std::unique_ptr<Scene> newScene) {
    PopScene(); // Pop the old scene
    PushScene(std::move(newScene)); // Push the new scene
}

void SceneManager::Update(bool& isRunning, float dt)
{
    if (!scenes.empty()) {
        scenes.top()->Update(isRunning, dt);
    }
}

void SceneManager::Draw() {
    if (!scenes.empty()) {
        scenes.top()->Draw();
    }
}
void SceneManager::HandleInput() {
    if (!scenes.empty()) {
        scenes.top()->HandleInput();
    }
}

// Get the top scene if needed (for certain checks)
Scene* SceneManager::GetCurrentScene() {
    return scenes.empty() ? nullptr : scenes.top().get();
}
