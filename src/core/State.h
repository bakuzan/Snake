#ifndef STATE_H
#define STATE_H

#include <SFML/Graphics.hpp>

class State
{
public:
    virtual ~State() = default;

    virtual void handleEvent(const sf::Event &event) = 0;
    virtual void update(sf::Time deltaTime) = 0;
    virtual void render() = 0;

    virtual void handleWindowResize(sf::Vector2u newSize) = 0;
};

#endif // STATE_H
