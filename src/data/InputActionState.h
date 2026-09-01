#pragma once

#include <SFML/System/Clock.hpp>

struct InputActionState
{
    bool primed = false;
    sf::Clock clock;

    static InputActionState makeDefault()
    {
        return {};
    }
};
