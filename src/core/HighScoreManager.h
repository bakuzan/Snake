#ifndef HIGHSCOREMANAGER_H
#define HIGHSCOREMANAGER_H

#include <string>
#include <vector>

#include "data/HighScore.h"

class HighScoreManager
{
private:
    std::vector<HighScore> highScores;

public:
    static constexpr size_t maxHighScores = 10;

public:
    HighScoreManager();
    ~HighScoreManager();

    void loadFromFile(const std::string &fileName);
    void saveToFile(const std::string &fileName);

    void addScore(const std::string &name, int score);
    const std::vector<HighScore> &getHighScores() const;
};

#endif // HIGHSCOREMANAGER_H
