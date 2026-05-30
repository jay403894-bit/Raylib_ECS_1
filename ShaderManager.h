#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>
#include "raylib.h"
class ShaderManager
{
private:
    static std::unordered_map<std::string, Shader> shaderCache;

public:
    // Load shader to cache and automatically bind it (Raylib's default behavior)
    static Shader LoadToCache(const char* vsFileName, const char* fsFileName);
    // Optionally, you could add methods to handle unloading/shutdown
    static void UnloadFromCache(const std::string& shaderKey);
    // Retrieve a reference to a cached shader by its key
    static Shader& GetShader(const std::string& shaderKey);
    // Explicitly unload the shader from GPU and remove from cache
    static void DeleteShader(const std::string& shaderKey);
    // Clear all shaders in the cache (useful during cleanup or shutdown)
    static void ClearCache();

    // Destructor to clean up shaders when shutting down
    ~ShaderManager();
};
