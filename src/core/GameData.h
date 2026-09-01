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
#include "core/SettingsManager.h"

class GameData
{
private:
public:
    sf::Font gameFont;

    TextureManager textureManager;
    AudioManager audioManager;
    SettingsManager settingsManager;

public:
    GameData();
    ~GameData();
};

#endif // GAMEDATA_H
