#ifndef GAMEUTILS_H
#define GAMEUTILS_H

#include <SFML/Graphics.hpp>
#include <unordered_map>

namespace GameUtils
{
    std::string padNumberAsText(int value, int width, char padChar);

    sf::Vector2f normaliseVector(const sf::Vector2f &vector);

    float getDistanceBetween(const sf::Vector2f &a,
                             const sf::Vector2f &b);

    std::string getPropertyOrDefault(const std::unordered_map<std::string, std::string> &props,
                                     const std::string &key,
                                     const std::string &defaultValue);

    inline sf::Vector2f getCentre(const sf::FloatRect &bounds)
    {
        return sf::Vector2f(bounds.left + bounds.width * 0.5f,
                            bounds.top + bounds.height * 0.5f);
    }

    template <typename T>
    inline sf::Vector2f getCentre(const T &sfmlObject)
    {
        return getCentre(sfmlObject.getGlobalBounds());
    }
};

#endif // GAMEUTILS_H
