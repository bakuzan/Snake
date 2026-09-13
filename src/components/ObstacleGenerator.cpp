#include <cstdlib>

#include "ObstacleGenerator.h"

sf::Vector2i ObstacleGenerator::generateLayout(std::vector<sf::Vector2i> &holes,
                                               sf::Vector2i gridBounds)
{
    holes.clear();
    int choice = std::rand() % 4;

    int cx = gridBounds.x / 2;
    int cy = gridBounds.y / 2;

    sf::Vector2i spawnPos(cx, cy); // Default to dead center

    switch (choice)
    {
    case 0: // Central Cross (+)
        for (int x = cx - 7; x <= cx + 7; ++x)
            holes.push_back({x, cy});
        for (int y = cy - 7; y <= cy + 7; ++y)
            holes.push_back({cx, y});

        // Center is blocked! Spawn in the top-left quadrant instead.
        spawnPos = sf::Vector2i(cx / 2, cy / 2);
        break;

    case 1: // Parallel Vertical Tunnels
        for (int y = 4; y < gridBounds.y - 4; ++y)
        {
            holes.push_back({cx - 7, y});
            holes.push_back({cx + 7, y});
        }
        // Spawn dead center (middle lane)
        spawnPos = sf::Vector2i(cx, cy);
        break;

    case 2: // Four Corners Brackets
        for (int i = 3; i < 10; ++i)
        {
            holes.push_back({i, 3});
            holes.push_back({3, i});

            holes.push_back({gridBounds.x - 1 - i, 3});
            holes.push_back({gridBounds.x - 4, i});

            holes.push_back({i, gridBounds.y - 4});
            holes.push_back({3, gridBounds.y - 1 - i});

            holes.push_back({gridBounds.x - 1 - i, gridBounds.y - 4});
            holes.push_back({gridBounds.x - 4, gridBounds.y - 1 - i});
        }
        // Center is wide open
        spawnPos = sf::Vector2i(cx, cy);
        break;

    case 3: // The U-Trap
        for (int y = cy - 6; y <= cy + 6; ++y)
            holes.push_back({cx - 6, y});
        for (int x = cx - 6; x <= cx + 6; ++x)
        {
            holes.push_back({x, cy - 6});
            holes.push_back({x, cy + 6});
        }
        // Center is safely nestled inside the U-shape
        spawnPos = sf::Vector2i(cx, cy);
        break;
    }

    return spawnPos;
}