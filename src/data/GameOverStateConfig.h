#pragma once

#include <SFML/Graphics.hpp>

#include <string>

struct GameOverStateConfig
{
    std::string titleText;
    sf::Color titleTextColour;

    static GameOverStateConfig init(std::string title, sf::Color titleColour)
    {
        return {title, titleColour};
    }

    static GameOverStateConfig defaultValues()
    {
        return {"Game Over", sf::Color::Red};
    }
};