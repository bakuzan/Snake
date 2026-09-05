#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>

#include "SettingsManager.h"

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
            if (key == "wrapAroundEnabled")
            {
                wrapAroundEnabled = (value == "1");
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
    file << "wrapAroundEnabled=" << (wrapAroundEnabled ? "1" : "0") << "\n";

    file.close();
}

void SettingsManager::reset()
{
    load();
}

std::string SettingsManager::getModeName() const
{
    std::vector<std::string> activeMods;

    if (wrapAroundEnabled)
    {
        activeMods.push_back("WRAP");
    }

    if (activeMods.empty())
    {
        return "CLASSIC";
    }

    std::string result = activeMods[0];
    for (size_t i = 1; i < activeMods.size(); ++i)
    {
        result += " + " + activeMods[i];
    }

    return result;
}

std::string SettingsManager::getScoreFilename() const
{
    std::string scoreFilename = "highscores";

    if (wrapAroundEnabled)
    {
        scoreFilename += "_wrap";
    }

    return scoreFilename + ".txt";
}

// Privates

void SettingsManager::restoreDefaults()
{
    wrapAroundEnabled = false;
}