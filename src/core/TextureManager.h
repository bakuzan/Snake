#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SFML/Graphics.hpp>

#include <unordered_map>

#include "constants/TextureId.h"

class TextureManager
{
private:
    std::unordered_map<TextureId, sf::Texture> textures;

public:
    TextureManager();
    ~TextureManager();

    bool loadTexture(TextureId id, const std::string &filepath, bool isRepeated = false);
    const sf::Texture &getTexture(TextureId id) const;
    bool hasTexture(TextureId id) const;
};

#endif // TEXTUREMANAGER_H
