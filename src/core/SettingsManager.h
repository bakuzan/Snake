#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <string>

class SettingsManager
{
private:
    std::string filename = "settings.txt";

private:
    void restoreDefaults();

public:
    // Data
    bool showEnemyHealthBars = true;

public:
    SettingsManager();
    ~SettingsManager() = default;

    void load();
    void save();
    void reset();
};

#endif // SETTINGSMANAGER_H
