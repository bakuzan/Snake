#pragma once

#include <ctime>
#include <string>

struct HighScore
{
    std::string name;
    int score;
    std::time_t timestamp;
};
