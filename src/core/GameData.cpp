#include <utility>

#include "GameData.h"

GameData::GameData()
{
    // Load textures

    // Load audio

    // Load font
    if (!gameFont.loadFromFile("resources/fonts/PressStart2P-Regular.ttf"))
    {
        throw std::runtime_error("Failed to load font");
    }
}

GameData::~GameData()
{
    // Destructor
}

// Publics

// Resetting
void GameData::reset()
{
}
