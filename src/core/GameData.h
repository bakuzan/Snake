#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <SFML/Graphics.hpp>

#include <optional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "TextureManager.h"
#include "AudioManager.h"
#include "constants/GameMode.h"
#include "core/SettingsManager.h"

class GameData
{
private:
public:
    sf::Font gameFont;

    TextureManager textureManager;
    AudioManager audioManager;
    SettingsManager settingsManager;

    GameMode mode;

public:
    GameData();
    ~GameData();

    GameMode getGameMode() const;
    void setGameMode(GameMode chosenMode);

    // Resetters
    void reset();
};

#endif // GAMEDATA_H
