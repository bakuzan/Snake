#ifndef SNAKE_H
#define SNAKE_H

#include <SFML/Graphics.hpp>
#include <deque>
#include <queue>

#include "constants/Direction.h"

class Snake
{
private:
    float cellSize;

public:
    Snake(float cellSize, sf::Vector2i startGridPos, int initialLength = 3);
    ~Snake();

    void handleInput(Direction dir);
    void update();
    void grow();

    bool checkSelfCollision() const;
    sf::Vector2i getHeadPosition() const;
    void setHeadPosition(sf::Vector2i position);
    const std::deque<sf::Vector2i> &getSegments() const;

    void render(sf::RenderTarget &target, int ghostTicksRemaining = 0) const;

private:
    std::deque<sf::Vector2i> body;
    Direction currentDirection{Direction::RIGHT};
    std::queue<Direction> directionQueue;
    bool pendingGrowth{false};

    bool isOpposite(Direction dir1, Direction dir2) const;
};

#endif // SNAKE_H
