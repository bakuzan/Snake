#include "constants/Constants.h"

#include "Snake.h"

Snake::Snake(float cellSize, sf::Vector2i startGridPos, int initialLength)
    : cellSize(cellSize)
{
    for (int i = 0; i < initialLength; ++i)
    {
        body.push_back({startGridPos.x - i, startGridPos.y});
    }
}

Snake::~Snake()
{
    // Destructor
}

// Publics

void Snake::handleInput(Direction dir)
{
    // Limit buffer depth to 2 to prevent stale turns
    if (directionQueue.size() < 2)
    {
        directionQueue.push(dir);
    }
}

void Snake::update()
{
    while (!directionQueue.empty())
    {
        Direction nextDir = directionQueue.front();
        directionQueue.pop();

        if (!isOpposite(currentDirection, nextDir))
        {
            currentDirection = nextDir;
            break;
        }
    }

    // Calculate new head position
    sf::Vector2i newHead = body.front();
    switch (currentDirection)
    {
    case Direction::UP:
        newHead.y -= 1;
        break;
    case Direction::DOWN:
        newHead.y += 1;
        break;
    case Direction::LEFT:
        newHead.x -= 1;
        break;
    case Direction::RIGHT:
        newHead.x += 1;
        break;
    }

    body.push_front(newHead);

    if (pendingGrowth)
    {
        pendingGrowth = false;
    }
    else
    {
        body.pop_back(); // Remove tail unless growing
    }
}

void Snake::grow()
{
    pendingGrowth = true;
}

bool Snake::checkSelfCollision() const
{
    const auto &head = body.front();

    for (size_t i = 1; i < body.size(); ++i)
    {
        if (body[i] == head)
        {
            return true;
        }
    }

    return false;
}

sf::Vector2i Snake::getHeadPosition() const
{
    return body.front();
}

void Snake::setHeadPosition(sf::Vector2i position)
{
    if (!body.empty())
    {
        body.front() = position;
    }
}

const std::deque<sf::Vector2i> &Snake::getSegments() const
{
    return body;
}

bool Snake::isOpposite(Direction dir1, Direction dir2) const
{
    return (dir1 == Direction::UP && dir2 == Direction::DOWN) ||
           (dir1 == Direction::DOWN && dir2 == Direction::UP) ||
           (dir1 == Direction::LEFT && dir2 == Direction::RIGHT) ||
           (dir1 == Direction::RIGHT && dir2 == Direction::LEFT);
}

void Snake::render(sf::RenderTarget &target,
                   int ghostTicksRemaining) const
{
    bool isGhost = ghostTicksRemaining > 0;
    bool isFlashing = isGhost &&
                      ghostTicksRemaining <= 10 &&
                      (ghostTicksRemaining % 4 < 2);

    sf::Color colour = Constants::snakeColour;

    if (isGhost)
    {
        if (isFlashing)
        {
            colour = Constants::snakeColour;
            colour.a = Constants::ghostColour.a;
        }
        else
        {
            colour = Constants::ghostColour;
        }
    }

    sf::RectangleShape shape(sf::Vector2f(cellSize - 1.f, cellSize - 1.f));
    shape.setFillColor(colour);

    for (size_t i = 0; i < body.size(); ++i)
    {
        shape.setPosition(body[i].x * cellSize, body[i].y * cellSize);
        target.draw(shape);
    }
}