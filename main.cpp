#include <raylib.h>
#include "SceneManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "ShaderManager.h"
#include "TaskManager/TaskManager.h"
#include "Scenes.h"
#include "Utilities/Clock.h"


void initialize() {
    SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(800, 600, "Hello, World!");
	SetTargetFPS(60);
	InitAudioDevice();
	FontManager::Load("telegrama_raw", "telegrama_raw.otf", 32);
	SceneManager::PushScene(std::make_unique<StartMenu>());


	SetTargetFPS(60);
}

int main() {
    initialize();

    bool isRunning = true;

    while (isRunning)
    {
       // TaskManager::ProcessMainThreadTasks();

        BeginDrawing();
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;  // cap at 50ms
        SceneManager::Update(isRunning, dt);
        SceneManager::HandleInput();
        SceneManager::Draw();

        EndDrawing();
    }

    SceneManager::PopScene();
    CloseWindow();
    return 0;
}