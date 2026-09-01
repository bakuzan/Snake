#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "GameUtils.h"

namespace GameUtils
{
    std::string padNumberAsText(int value, int width, char padChar)
    {
        std::stringstream ss;
        ss << std::setw(width) << std::setfill(padChar) << value;
        return ss.str();
    }

    sf::Vector2f normaliseVector(const sf::Vector2f &vector)
    {
        float magnitude = std::sqrt(vector.x * vector.x + vector.y * vector.y);
        if (magnitude > 0)
        {
            return vector / magnitude;
        }

        return sf::Vector2f(0.0f, 0.0f);
    }

    float getDistanceBetween(const sf::Vector2f &a,
                             const sf::Vector2f &b)
    {
        sf::Vector2f d = a - b;
        return std::sqrt(d.x * d.x + d.y * d.y);
    }

    std::string getPropertyOrDefault(const std::unordered_map<std::string, std::string> &props,
                                     const std::string &key,
                                     const std::string &defaultValue)
    {
        if (props.contains(key))
        {
            return props.at(key);
        }

        return defaultValue;
    }
}