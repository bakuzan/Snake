#include <algorithm>
#include <fstream>

#include "HighScoreManager.h"

HighScoreManager::HighScoreManager()
{
    // Constructor
}

HighScoreManager::~HighScoreManager()
{
    // Destructor
}

// Publics

void HighScoreManager::loadFromFile(const std::string &fileName)
{
    std::ifstream file(fileName);
    highScores.clear();

    std::string line;
    while (std::getline(file, line))
    {
        size_t firstDelimiter = line.find('|');
        size_t secondDelimiter = line.find('|', firstDelimiter + 1);

        if (firstDelimiter != std::string::npos &&
            secondDelimiter != std::string::npos)
        {
            std::string name = line.substr(0, firstDelimiter);
            int score = std::stoi(line.substr(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1));
            std::time_t timestamp = std::stoll(line.substr(secondDelimiter + 1));

            highScores.push_back({name, score, timestamp});
        }
    }

    std::sort(highScores.begin(), highScores.end(),
              [](const HighScore &a, const HighScore &b)
              { return a.score > b.score; });
}

void HighScoreManager::saveToFile(const std::string &fileName)
{
    std::ofstream file(fileName);
    for (size_t i = 0; i < std::min(highScores.size(), maxHighScores); ++i)
    {
        const HighScore &entry = highScores[i];
        file << entry.name << "|"
             << entry.score << "|"
             << entry.timestamp << '\n';
    }
}

void HighScoreManager::addScore(const std::string &name, int score)
{
    std::time_t now = std::time(nullptr);
    highScores.push_back({name, score, now});
    std::sort(highScores.begin(), highScores.end(),
              [](const HighScore &a, const HighScore &b)
              { return a.score > b.score; });

    if (highScores.size() > maxHighScores)
    {
        highScores.pop_back();
    }
}

const std::vector<HighScore> &HighScoreManager::getHighScores() const
{
    return highScores;
}