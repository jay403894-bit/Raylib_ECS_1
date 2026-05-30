#pragma once
#include <stack>
#include <memory>

class Scene
{
public:
	Scene() = default;
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	virtual void Update(bool &isRunning, float dt = 0.0f) = 0;
	virtual void Draw() = 0;
	virtual void HandleInput() = 0;
	virtual  ~Scene() {};
};

class SceneManager {
private:
	static std::stack <std::unique_ptr<Scene>> scenes;
	SceneManager() = default;
public:
	// Delete copy constructor and assignment operator to prevent copies
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	// Static method to get the instance of the SceneManager
	static void PushScene(std::unique_ptr<Scene> newScene);
	static void PopScene();
	static void ReplaceScene(std::unique_ptr<Scene> newScene);
	static void Update(bool& isRunning,float dt = 0.0f);
	static void Draw();
	static void HandleInput();
	static Scene* GetCurrentScene();
};