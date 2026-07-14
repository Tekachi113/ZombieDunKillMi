#include "ResourceManager.h"
#include <iostream>

sf::Texture& ResourceManager::loadTexture(const std::string& id, const std::string& filepath) {
    // Return existing if already loaded
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }

    sf::Texture texture;
    if (!texture.loadFromFile(filepath)) {
        throw std::runtime_error("[ResourceManager] Failed to load texture: " + filepath);
    }

    // Pixel art: keep edges crisp, no smoothing
    texture.setSmooth(false);

    auto [inserted, success] = textures.emplace(id, std::move(texture));
    std::cout << "[ResourceManager] Loaded texture: " << id << " (" << filepath << ")" << std::endl;
    return inserted->second;
}

sf::Texture& ResourceManager::getTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it == textures.end()) {
        throw std::runtime_error("[ResourceManager] Texture not found: " + id);
    }
    return it->second;
}

bool ResourceManager::hasTexture(const std::string& id) const {
    return textures.find(id) != textures.end();
}

sf::Font& ResourceManager::loadFont(const std::string& id, const std::string& filepath) {
    auto it = fonts.find(id);
    if (it != fonts.end()) {
        return it->second;
    }

    sf::Font font;
    if (!font.loadFromFile(filepath)) {
        throw std::runtime_error("[ResourceManager] Failed to load font: " + filepath);
    }

    auto [inserted, success] = fonts.emplace(id, std::move(font));
    std::cout << "[ResourceManager] Loaded font: " << id << " (" << filepath << ")" << std::endl;
    return inserted->second;
}

sf::Font& ResourceManager::getFont(const std::string& id) {
    auto it = fonts.find(id);
    if (it == fonts.end()) {
        throw std::runtime_error("[ResourceManager] Font not found: " + id);
    }
    return it->second;
}

bool ResourceManager::hasFont(const std::string& id) const {
    return fonts.find(id) != fonts.end();
}

sf::SoundBuffer& ResourceManager::loadSoundBuffer(const std::string& id, const std::string& filepath) {
    auto it = soundBuffers.find(id);
    if (it != soundBuffers.end()) {
        return it->second;
    }

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filepath)) {
        throw std::runtime_error("[ResourceManager] Failed to load sound: " + filepath);
    }

    auto [inserted, success] = soundBuffers.emplace(id, std::move(buffer));
    std::cout << "[ResourceManager] Loaded sound: " << id << " (" << filepath << ")" << std::endl;
    return inserted->second;
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& id) {
    auto it = soundBuffers.find(id);
    if (it == soundBuffers.end()) {
        throw std::runtime_error("[ResourceManager] SoundBuffer not found: " + id);
    }
    return it->second;
}

bool ResourceManager::hasSoundBuffer(const std::string& id) const {
    return soundBuffers.find(id) != soundBuffers.end();
}
