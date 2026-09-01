#pragma once

#include <SFML/Graphics.hpp>

struct InputBinding
{
    sf::Keyboard::Key key;
    bool requiresDoublePress = false;
    float doublePressWindow = 0.25f;
};