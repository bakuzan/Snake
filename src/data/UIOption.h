#pragma once

#include <SFML/Graphics.hpp>

template <typename ValueType>
struct UIOption
{
    ValueType value;
    sf::RectangleShape box;
    sf::Text label;
};
