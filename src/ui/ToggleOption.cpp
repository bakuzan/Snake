#include "ToggleOption.h"

ToggleOption::ToggleOption(std::string name, std::string labelText,
                           bool *ref, const sf::Font &font, sf::Vector2f pos)
    : settingReference(ref),
      button(name, font, "Off", pos, [this]()
             {
          *settingReference = !(*settingReference);
          updateUI(); })
{
    label.setFont(font);
    label.setString(labelText);

    setPosition(pos);
}

// Publics

void ToggleOption::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

        if (button.isMouseOver(mousePos, window))
        {
            button.trigger();
        }
    }
}

void ToggleOption::render(sf::RenderWindow &win) const
{
    win.draw(label);
    button.render(win);
}

void ToggleOption::setPosition(sf::Vector2f pos)
{
    label.setPosition(pos);
    button.setPosition(sf::Vector2f(pos.x, pos.y + (label.getGlobalBounds().height)));

    updateUI();
}

// Privates

void ToggleOption::updateUI()
{
    if (*settingReference)
    {
        button.setText("On");
        button.setTextColour(sf::Color::Green);
    }
    else
    {
        button.setText("Off");
        button.setTextColour(sf::Color::Red);
    }
}