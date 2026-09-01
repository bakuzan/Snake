#pragma once

#include <SFML/Graphics.hpp>

#include <string>

struct SaveSlot
{
    sf::RectangleShape box;
    sf::Text label;
    bool occupied = false;
    std::string saveId; // or timestamp
};