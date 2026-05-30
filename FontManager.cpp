#include "FontManager.h"
std::unordered_map<std::string, Font> FontManager::fonts;

// FontManager.cpp
void FontManager::Load(const std::string& id, const std::string& filename, int size) {
    if (fonts.find(id) == fonts.end()) {
        // Load the font
        Font loadedFont = LoadFontEx(filename.c_str(), size, 0, 0);

        // Check if font loaded successfully by checking the texture ID
        if (loadedFont.texture.id == 0) {
            std::cerr << "Error: Failed to load font: " << filename << std::endl;
            // You could take further action here, like loading a default font
            return; // Exit early if font failed to Load
        }

        // If successful, store the font in the map
        fonts[id] = loadedFont;
    }
}
std::shared_ptr<Font> FontManager::Get(const std::string& id) {
    if (fonts.find(id) != fonts.end()) {
        return std::make_shared<Font>(fonts.at(id));
    }
    else
        return nullptr;
}
void FontManager::Unload(const std::string& id) {
    if (fonts.count(id) > 0) {
        UnloadFont(fonts.at(id));
        fonts.erase(id);
    }
}

void FontManager::UnloadAll() {
    for (auto& font : fonts) {
        UnloadFont(font.second);
    }
    fonts.clear();
}