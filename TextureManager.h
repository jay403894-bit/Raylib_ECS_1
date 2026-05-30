#pragma once
#include <string>
#include <unordered_map>
#include <raylib.h>
#include <mutex>
#include "TaskManager/TaskManager.h"
#include "TaskManager/Event.hpp"
class TextureLoadedEvent : public Event {
public:
    std::string get_event_type() const override {
        return "TextureLoaded";
    }
};

class TextureManager {
private:
    TextureManager() = default; // Singleton constructor
    static std::unordered_map<int, Texture2D> textures; // Map of textures by ID

public:
    static TextureLoadedEvent TextureLoaded; // static shared_ptr

    // Load a texture by its ID and file name
    static void LoadTextureID(const int& id, const std::string& fileName);

    // Get a texture by ID
    static Texture2D Get(const int& id);

    // Unload a specific texture
    static void UnloadTextureID(const int& id);

    // Unload all textures
    static void UnloadAllTextures();

};