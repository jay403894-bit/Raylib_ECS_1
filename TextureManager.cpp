#include "TextureManager.h"
std::unordered_map<int, Texture2D> TextureManager::textures; // Map of textures by ID

// Load a texture by its ID and file name
void TextureManager::LoadTextureID(const int& id, const std::string& fileName) {
    if (textures.find(id) == textures.end()) {
        textures[id] = LoadTexture(fileName.c_str());
    }
}

// Get a texture by ID
Texture2D TextureManager::Get(const int& id) {
    if (textures.find(id) != textures.end()) {
        return textures[id];
    }
    return Texture2D{}; // Return an empty texture if not found
}

// Unload a specific texture
void TextureManager::UnloadTextureID(const int& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        UnloadTexture(it->second);
        textures.erase(it);
    }
}

// Unload all textures
void TextureManager::UnloadAllTextures() {
    for (auto& texture : textures) {
        UnloadTexture(texture.second);
    }
    textures.clear();
}
