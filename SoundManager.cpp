#include "SoundManager.h"
std::unordered_map<std::string, Sound> SoundManager::sounds;  // Map for sound effects by ID
std::unordered_map<std::string, Music> SoundManager::music;   // Map for music tracks by ID


// Load sound effect from file
void SoundManager::LoadSoundID(const std::string& id, const std::string& fileName) {
    if (sounds.find(id) == sounds.end()) {
        sounds[id] = LoadSound(fileName.c_str());
    }
}

// Load music from file
void SoundManager::LoadMusicID(const std::string& id, const std::string& fileName) {
    if (music.find(id) == music.end()) {
        music[id] = LoadMusicStream(fileName.c_str());
    }
}

// Play a sound effect by ID
void SoundManager::PlaySoundID(const std::string& id) {
    if (sounds.find(id) != sounds.end()) {
        PlaySound(sounds[id]);
    }
}

// Play music by ID
void SoundManager::PlayMusicID(const std::string& id, bool loop) {
    if (music.find(id) != music.end()) {
        PlayMusicStream(music[id]);
    }
}

// Stop a specific sound effect
void SoundManager::StopSoundID(const std::string& id) {
    if (sounds.find(id) != sounds.end()) {
        StopSound(sounds[id]);
    }
}

// Stop all sounds
void SoundManager::StopAllSounds() {
    for (auto& sound : sounds) {
        StopSound(sound.second);
    }
}

// Pause all music
void SoundManager::PauseMusic() {
    for (auto& musicTrack : music) {
        PauseMusicStream(musicTrack.second);
    }
}

// Resume all music
void SoundManager::ResumeMusic() {
    for (auto& musicTrack : music) {
        ResumeMusicStream(musicTrack.second);
    }
}

// Update music stream for playback
void SoundManager::UpdateMusicID(const std::string& id) {
    UpdateMusicStream(music[id]);
}

// Unload sound and music resources
void SoundManager::UnloadSoundID(const std::string& id) {
    if (sounds.find(id) != sounds.end()) {
        UnloadSound(sounds[id]);
        sounds.erase(id);
    }
}

void SoundManager::UnloadMusic(const std::string& id) {
    if (music.find(id) != music.end()) {
        UnloadMusicStream(music[id]);
        music.erase(id);
    }
}