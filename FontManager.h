#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <raylib.h>
class FontManager {
public:
    static void Load(const std::string& id, const std::string& filename, int size = 32);
    static std::shared_ptr<Font> Get(const std::string& id);
    static void Unload(const std::string& id);
    static void UnloadAll();

private:
    static std::unordered_map<std::string, Font> fonts;
};
