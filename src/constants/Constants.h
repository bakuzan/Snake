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
    constexpr float CELL_SIZE = 32.0f;
    constexpr int GRID_WIDTH = 25;

    // Other
    constexpr float MAX_SPEED_LIMIT = 0.05f;
    constexpr int FRUIT_POINT_VALUE = 100;
    constexpr int GHOST_TICKS_DURATION = 30;
    constexpr int SPECIAL_FRUIT_SPAWN_COUNTDOWN = 40;
    constexpr int SPECIAL_FRUIT_DURATION = 35;
    constexpr int MIN_PORTAL_DISTANCE = 10;
    constexpr float TIME_ATTACK_START_TIME = 60.f;
    constexpr float TIME_ATTACK_MAX_TIME = 90.f;
    constexpr float TIME_ATTACK_INCREMENT = 2.5f;

    // Colours
    const sf::Color cellColour(83, 86, 90);
    const sf::Color cellBorder(136, 139, 141);

    const sf::Color fruitColour(199, 55, 47);
    const sf::Color bonusFruitColour(sf::Color::Yellow);
    const sf::Color poisonFruitColour(sf::Color(128, 0, 128));
    const sf::Color ghostFruitColour(sf::Color::White);

    const sf::Color snakeColour(108, 187, 60);
    const sf::Color ghostColour(0, 255, 255, 128);

    const sf::Color pitColour(sf::Color::Black);
    const sf::Color pitBorder(sf::Color(60, 60, 60));

    const sf::Color portal1Colour(sf::Color(255, 165, 0));
    const sf::Color portal2Colour(sf::Color::Blue);

};

#endif // CONSTANTS_H
