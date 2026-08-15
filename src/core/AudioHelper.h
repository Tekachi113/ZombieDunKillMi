#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <unordered_map>

// =========================================================
//  AudioHelper — plays one-shot sound effects (gunfire, explosions...)
//  and a single looping background music track.
//
//  NOTE: the original project roadmap listed an "AudioManager" as
//  planned core/ infrastructure that never got built. This fills that
//  gap under a different name to avoid colliding if someone else adds
//  their own AudioManager later -- rename/merge them at that point.
//
//  Fully self-contained: doesn't depend on ResourceManager or any
//  other system, so it's safe to drop in without touching other files.
// =========================================================
class AudioHelper {
public:
    // Fire-and-forget sound effect. Safe to call rapidly/overlapping
    // (e.g. shotgun pellets, fast pistol fire) -- each call gets its
    // own sf::Sound instance from an internal pool.
    static void playSfx(const std::string& filepath, float volume = 100.f);

    // Background music: only one track plays at a time.
    static void playMusic(const std::string& filepath, bool loop = true);
    static void stopMusic();

    // Menu on/off toggle. When disabled, remembers the current track
    // and resumes it from the top if re-enabled later.
    static void setMusicEnabled(bool enabled);
    static bool isMusicEnabled();

    static void setMusicVolume(float volume); // 0-100
    static void setSfxVolume(float volume);   // 0-100, applied to future playSfx calls

private:
    static sf::SoundBuffer& getBuffer(const std::string& filepath);

    static std::unordered_map<std::string, sf::SoundBuffer> buffers;
    static std::vector<sf::Sound> activeSounds; // pool, pruned each playSfx() call

    static sf::Music music;
    static bool musicEnabled;
    static float musicVolume;
    static float sfxVolume;
    static std::string currentMusicPath;
};
