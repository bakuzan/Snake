#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>
#include <string>

namespace Constants
{
    const std::string PROJECT_NAME = "Snake";

    // Dimensions
    constexpr float VIEW_WIDTH = 960.0f;
    constexpr float VIEW_HEIGHT = 540.0f;
    constexpr float BUTTON_WIDTH = 250.0f;
    constexpr float BUTTON_HEIGHT = 50.0f;
    constexpr float TILE_SIZE = 32.0f;

    // Colours
    const sf::Color fruitColour(199, 55, 47);
    const sf::Color headColour(108, 187, 60);
    const sf::Color bodyColour(121, 196, 74);

};

#endif // CONSTANTS_H
