#ifndef TOGGLEOPTION_H
#define TOGGLEOPTION_H

#include <SFML/Graphics.hpp>
#include <string>

#include "Button.h"

class ToggleOption
{
private:
    sf::Text label;
    Button button;
    bool *settingReference;

    void updateUI();

public:
    ToggleOption(std::string name, std::string labelText,
                 bool *ref, const sf::Font &font, sf::Vector2f pos);

    void handleEvent(const sf::Event &event, const sf::RenderWindow &window);
    void render(sf::RenderWindow &win) const;

    void setPosition(sf::Vector2f pos);
};

#endif // TOGGLEOPTION_H