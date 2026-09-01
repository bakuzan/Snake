#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "ui/Button.h"

namespace InputUtils
{
    bool isAnyKeyPressed(const std::vector<sf::Keyboard::Key> &keys);
    bool isAnyKeyPressed(sf::Keyboard::Key pressedKey, const std::vector<sf::Keyboard::Key> &keys);
    void handleButtonEvent(const sf::Event &event,
                           std::vector<Button> &buttons,
                           sf::RenderWindow &window,
                           int &selectedButtonIndex);
}

#endif // INPUTUTILS_H
