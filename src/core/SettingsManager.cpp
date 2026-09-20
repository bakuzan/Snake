#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>

#include "SettingsManager.h"

namespace
{
    struct SettingDef
    {
        bool SettingsManager::*memberPtr;
        const char *fileKey;
        const char *modeName;
        const char *scoreSuffix;
    };

    const SettingDef settingsConfig[] = {
        {&SettingsManager::wrapAroundEnabled, "wrapAroundEnabled", "WRAP", "_wrap"},
        {&SettingsManager::holesEnabled, "holesEnabled", "HOLES", "_holes"},
        {&SettingsManager::speedUpEnabled, "speedUpEnabled", "FAST", "_fast"},
        {&SettingsManager::specialFoodEnabled, "specialFoodEnabled", "SP FRUIT", "_fruit"},
        {&SettingsManager::portalsEnabled, "portalsEnabled", "PORTALS", "_portals"},
        {&SettingsManager::obstaclesEnabled, "obstaclesEnabled", "OBSTACLES", "_obstacles"},
        {&SettingsManager::strictGhostTelefragEnabled, "strictGhostTelefragEnabled", "TELEFRAG", "_telefrag"}};
}

SettingsManager::SettingsManager()
{
    load();
}

// Publics

void SettingsManager::load()
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        restoreDefaults();
        return;
    }

    std::string line;
    std::unordered_map<std::string, std::string> kvStore;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(iss, key, '=') &&
            std::getline(iss, value))
        {
            for (const auto &config : settingsConfig)
            {
                if (key == config.fileKey)
                {
                    this->*(config.memberPtr) = (value == "1");
                    break;
                }
            }
        }
    }
}

void SettingsManager::save()
{
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open())
    {
        std::cerr << "[SettingsManager] Could not open settings file for writing: "
                  << filename << "\n";
        return;
    }

    // Serialize settings as key=value pairs
    for (const auto &config : settingsConfig)
    {
        file << config.fileKey << "="
             << (this->*(config.memberPtr) ? "1" : "0")
             << "\n";
    }

    file.close();
}

void SettingsManager::reset()
{
    load();
}

std::string SettingsManager::getModeName(GameMode currentMode) const
{
    std::string modifiers = "";
    std::string uiName = (currentMode == GameMode::TIME_ATTACK)
                             ? "TIME ATTACK"
                             : "CLASSIC";

    bool first = true;
    for (const auto &config : settingsConfig)
    {
        if (this->*(config.memberPtr))
        {
            if (!first)
            {
                modifiers += " + ";
            }

            modifiers += config.modeName;
            first = false;
        }
    }

    if (!modifiers.empty())
    {
        uiName += " [" + modifiers + "]";
    }

    return uiName;
}

std::string SettingsManager::getScoreFilename(GameMode currentMode) const
{
    std::string scoreFilename = (currentMode == GameMode::TIME_ATTACK)
                                    ? "highscores_timeattack"
                                    : "highscores";

    for (const auto &config : settingsConfig)
    {
        if (this->*(config.memberPtr))
        {
            scoreFilename += config.scoreSuffix;
        }
    }

    return scoreFilename + ".txt";
}

// Privates

void SettingsManager::restoreDefaults()
{
    for (const auto &config : settingsConfig)
    {
        this->*(config.memberPtr) = false;
    }
}