#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SFML/Graphics.hpp>

#include <unordered_map>

#include "constants/Action.h"
#include "data/InputActionState.h"
#include "data/InputBinding.h"

class InputManager
{
private:
    std::unordered_map<Action, InputBinding> keyBindings;
    std::unordered_map<Action, InputActionState> actionStates;

public:
    InputManager();
    ~InputManager();

    void bind(Action action, sf::Keyboard::Key key,
              bool requiresDoublePress = false,
              float window = 0.25f);
    bool isDown(Action action) const;
    bool isPressed(Action action, sf::Keyboard::Key pressedKey);
    std::string getKeyName(Action action) const;

private:
    static std::string keyToString(sf::Keyboard::Key key)
    {
        switch (key)
        {
        case sf::Keyboard::W:
            return "W";
        case sf::Keyboard::A:
            return "A";
        case sf::Keyboard::S:
            return "S";
        case sf::Keyboard::D:
            return "D";
        case sf::Keyboard::Escape:
            return "Esc";
        case sf::Keyboard::Space:
            return "Space";
        case sf::Keyboard::Enter:
            return "Enter";
        default:
            return "?";
        }
    }
};

#endif // INPUTMANAGER_H
