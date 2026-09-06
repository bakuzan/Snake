#include "constants/Constants.h"

#include "Food.h"

Food::Food(float cellSize)
    : cellSize(cellSize)
{
    // Constructor
}

Food::~Food()
{
    // Destructor
}

// Publics

void Food::respawn(sf::Vector2i gridBounds,
                   const std::deque<sf::Vector2i> &snakeSegments,
                   const std::vector<sf::Vector2i> &holes)
{
    std::uniform_int_distribution<int> distX(0, gridBounds.x - 1);
    std::uniform_int_distribution<int> distY(0, gridBounds.y - 1);

    bool validPosition = false;
    while (!validPosition)
    {
        position = {distX(rng), distY(rng)};
        validPosition = true;

        for (const auto &segment : snakeSegments)
        {
            if (segment == position)
            {
                validPosition = false;
                break;
            }
        }

        if (!validPosition)
        {
            continue;
        }

        for (const auto &hole : holes)
        {
            if (hole == position)
            {
                validPosition = false;
                break;
            }
        }
    }
}

sf::Vector2i Food::getPosition() const
{
    return position;
}

void Food::render(sf::RenderTarget &target) const
{
    sf::RectangleShape shape(sf::Vector2f(cellSize - 1.f, cellSize - 1.f));
    shape.setFillColor(Constants::fruitColour);
    shape.setPosition(position.x * cellSize, position.y * cellSize);
    target.draw(shape);
}