#ifndef OBSTACLEGENERATOR_H
#define OBSTACLEGENERATOR_H

#include <SFML/System/Vector2.hpp>
#include <vector>

class ObstacleGenerator
{
public:
    static sf::Vector2i generateLayout(std::vector<sf::Vector2i> &holes,
                                       sf::Vector2i gridBounds);
};

#endif // OBSTACLEGENERATOR_H
