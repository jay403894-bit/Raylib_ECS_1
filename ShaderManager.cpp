#include "ShaderManager.h"
std::unordered_map<std::string, Shader> ShaderManager::shaderCache;

// Load shader to cache and automatically bind it (Raylib's default behavior)
Shader ShaderManager::LoadToCache(const char* vsFileName, const char* fsFileName)
{
    std::string shaderKey = std::string(vsFileName) + "_" + std::string(fsFileName);

    // Check if the shader is already cached
    if (shaderCache.find(shaderKey) != shaderCache.end())
    {
        return shaderCache[shaderKey];  // Return cached shader
    }
    else
    {
        // Load and bind the shader, cache it for future use
        Shader newShader = LoadShader(vsFileName, fsFileName);
        shaderCache[shaderKey] = newShader;  // Cache the newly loaded shader
        return newShader;
    }
}
// Optionally, you could add methods to handle unloading/shutdown
void ShaderManager::UnloadFromCache(const std::string& shaderKey)
{
    if (shaderCache.find(shaderKey) != shaderCache.end())
    {
        UnloadShader(shaderCache[shaderKey]); // Unload the shader from GPU
    }
}

// Retrieve a reference to a cached shader by its key
Shader& ShaderManager::GetShader(const std::string& shaderKey)
{
    if (shaderCache.find(shaderKey) != shaderCache.end())
    {
        return shaderCache[shaderKey]; // Return reference to cached shader
    }
    else
    {
        // Handle case where shader is not found in cache
        throw std::runtime_error("Shader not found in cache.");
    }
}

// Explicitly unload the shader from GPU and remove from cache
void ShaderManager::DeleteShader(const std::string& shaderKey)
{
    if (shaderCache.find(shaderKey) != shaderCache.end())
    {
        UnloadShader(shaderCache[shaderKey]);  // Unload shader from GPU
        shaderCache.erase(shaderKey);          // Remove shader from cache
    }
    else
    {
        // Optionally log or handle error: Shader not found in cache
    }
}

// Clear all shaders in the cache (useful during cleanup or shutdown)
void ShaderManager::ClearCache()
{
    for (auto& entry : shaderCache)
    {
        UnloadShader(entry.second); // Unload each shader
    }
    shaderCache.clear(); // Clear the cache
}

// Destructor to clean up shaders when shutting down
ShaderManager::~ShaderManager()
{
    ClearCache();
}