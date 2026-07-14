#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

class ResourceManager {
public:
    // Load and cache a texture (pixel art: smooth = false)
    sf::Texture& loadTexture(const std::string& id, const std::string& filepath);
    sf::Texture& getTexture(const std::string& id);
    bool hasTexture(const std::string& id) const;

    // Load and cache a font
    sf::Font& loadFont(const std::string& id, const std::string& filepath);
    sf::Font& getFont(const std::string& id);
    bool hasFont(const std::string& id) const;

    // Load and cache a sound buffer
    sf::SoundBuffer& loadSoundBuffer(const std::string& id, const std::string& filepath);
    sf::SoundBuffer& getSoundBuffer(const std::string& id);
    bool hasSoundBuffer(const std::string& id) const;

private:
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
};
