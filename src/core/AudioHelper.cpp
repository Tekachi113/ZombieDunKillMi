#include "AudioHelper.h"
#include <algorithm>
#include <iostream>

std::unordered_map<std::string, sf::SoundBuffer> AudioHelper::buffers;
std::vector<sf::Sound> AudioHelper::activeSounds;

sf::Music AudioHelper::music;
bool AudioHelper::musicEnabled = true;
float AudioHelper::musicVolume = 60.f;
float AudioHelper::sfxVolume = 80.f;
std::string AudioHelper::currentMusicPath;

sf::SoundBuffer& AudioHelper::getBuffer(const std::string& filepath) {
    auto it = buffers.find(filepath);
    if (it != buffers.end()) return it->second;

    sf::SoundBuffer buf;
    if (!buf.loadFromFile(filepath)) {
        std::cerr << "[AudioHelper] Failed to load sound: " << filepath << "\n";
    }
    auto [inserted, ok] = buffers.emplace(filepath, std::move(buf));
    return inserted->second;
}

void AudioHelper::playSfx(const std::string& filepath, float volume) {
    sf::SoundBuffer& buf = getBuffer(filepath);

    // Prune sounds that finished playing so the pool doesn't grow
    // forever over a long play session.
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Status::Stopped; }),
        activeSounds.end());

    activeSounds.emplace_back(buf);
    activeSounds.back().setVolume(volume * (sfxVolume / 100.f));
    activeSounds.back().play();
}

void AudioHelper::playMusic(const std::string& filepath, bool loop) {
    currentMusicPath = filepath;
    if (!musicEnabled) return; // remembered, will start once re-enabled

    if (!music.openFromFile(filepath)) {
        std::cerr << "[AudioHelper] Failed to load music: " << filepath << "\n";
        return;
    }
    music.setLooping(loop);
    music.setVolume(musicVolume);
    music.play();
}

void AudioHelper::stopMusic() {
    music.stop();
}

void AudioHelper::setMusicEnabled(bool enabled) {
    musicEnabled = enabled;
    if (!enabled) {
        music.stop();
    }
    else if (!currentMusicPath.empty()) {
        if (music.openFromFile(currentMusicPath)) {
            music.setLooping(true);
            music.setVolume(musicVolume);
            music.play();
        }
    }
}

bool AudioHelper::isMusicEnabled() {
    return musicEnabled;
}

void AudioHelper::setMusicVolume(float volume) {
    musicVolume = volume;
    music.setVolume(musicVolume);
}

void AudioHelper::setSfxVolume(float volume) {
    sfxVolume = volume;
}
