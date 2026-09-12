#ifndef FOOD_H
#define FOOD_H

#include <SFML/Graphics.hpp>
#include <deque>
#include <random>

class Food
{
private:
    float cellSize;

public:
    Food(float cellSize);
    ~Food();

    void respawn(sf::Vector2i gridBounds,
                 const std::deque<sf::Vector2i> &snakeSegments,
                 const std::vector<sf::Vector2i> &holes);
    sf::Vector2i getPosition() const;
    void render(sf::RenderTarget &target,
                sf::Color foodColour = Constants::fruitColour) const;

private:
    sf::Vector2i position{0, 0};
    std::mt19937 rng{std::random_device{}()};
};

#endif // FOOD_H
