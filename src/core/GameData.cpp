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

GameMode GameData::getGameMode() const
{
    return mode;
}

void GameData::setGameMode(GameMode chosenMode)
{
    mode = chosenMode;
}

// Resetting
void GameData::reset()
{
}
