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
    bool wrapAroundEnabled{false};
    bool holesEnabled{false};
    bool speedUpEnabled{false};
    bool specialFoodEnabled{false};
    bool portalsEnabled{false};
    bool obstaclesEnabled{false};

public:
    SettingsManager();
    ~SettingsManager() = default;

    void load();
    void save();
    void reset();

    std::string getModeName() const;
    std::string getScoreFilename() const;
};

#endif // SETTINGSMANAGER_H
