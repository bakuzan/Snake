#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>

#include <functional>

class Button
{
private:
    std::string name;
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> action;

    sf::Color textColour;

private:
    void updateText(const std::string &label, const sf::Color &colour);

public:
    Button(const std::string &name,
           const sf::Font &font,
           const std::string &label,
           sf::Vector2f position,
           std::function<void()> action);
    ~Button();

    void render(sf::RenderWindow &window) const;
    void onHover(bool isHovered);
    bool isMouseOver(sf::Vector2i mousePosition, const sf::RenderWindow &window) const;
    void trigger() const;

    const sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f position);

    const std::string getName() const;

    void setText(const std::string &newText);
    void setTextColour(const sf::Color &colour);
};

#endif // BUTTON_H
