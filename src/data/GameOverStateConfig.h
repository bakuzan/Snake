#pragma once

#include <SFML/Graphics.hpp>

#include <string>

struct GameOverStateConfig
{
    std::string titleText;
    sf::Color titleTextColour;
    int score{0};

    static GameOverStateConfig init(std::string title, sf::Color titleColour, int gameScore = 0)
    {
        return {title, titleColour, gameScore};
    }

    static GameOverStateConfig defaultValues(int gameScore = 0)
    {
        return {"Game Over", sf::Color::Red, gameScore};
    }
};