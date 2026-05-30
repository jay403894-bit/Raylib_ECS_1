#pragma once
#include <unordered_map>
#include <string>
#include <raylib.h>

class SoundManager {
private:
    SoundManager() = default; // Private constructor for Singleton pattern

  static std::unordered_map<std::string, Sound> sounds;  // Map for sound effects by ID
  static std::unordered_map<std::string, Music> music;   // Map for music tracks by ID

public:
    
    // Prevent copying of the singleton
    SoundManager(SoundManager const&) = delete;
    void operator=(SoundManager const&) = delete;

    // Load sound effect from file
    static void LoadSoundID(const std::string& id, const std::string& fileName);

    // Load music from file
    static void LoadMusicID(const std::string& id, const std::string& fileName);

    // Play a sound effect by ID
    static void PlaySoundID(const std::string& id);

    // Play music by ID
    static void PlayMusicID(const std::string& id, bool loop = true);

    // Stop a specific sound effect
    static void StopSoundID(const std::string& id);

    // Stop all sounds
    static void StopAllSounds();

    // Pause all music
    static void PauseMusic();

    // Resume all music
    static void ResumeMusic();

    // Update music stream for playback
    static void UpdateMusicID(const std::string& id);

    // Unload sound and music resources
    static void UnloadSoundID(const std::string& id);

    // Unload music resources
    static void UnloadMusic(const std::string& id);
};