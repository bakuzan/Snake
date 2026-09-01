#include "TextureManager.h"

TextureManager::TextureManager()
{
    // Constructor
}

TextureManager::~TextureManager()
{
    // Destructor
}

// Publics

bool TextureManager::loadTexture(TextureId id, const std::string &filepath, bool isRepeated)
{
    if (textures.find(id) == textures.end())
    {
        sf::Texture texture;
        if (!texture.loadFromFile(filepath))
        {
            return false; // Failed to load
        }

        texture.setRepeated(isRepeated);
        textures[id] = std::move(texture);
    }

    return true;
}

const sf::Texture &TextureManager::getTexture(TextureId id) const
{
    auto it = textures.find(id);
    if (it != textures.end())
    {
        return it->second;
    }

    throw std::runtime_error("Texture not found: " + std::to_string(static_cast<int>(id)));
}

bool TextureManager::hasTexture(TextureId id) const
{
    return textures.find(id) != textures.end();
}