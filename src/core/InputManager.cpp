#include "InputManager.h"

InputManager::InputManager()
{
    // Constructor
}

InputManager::~InputManager()
{
    // Destructor
}

// Publics

void InputManager::bind(Action action, sf::Keyboard::Key key,
                        bool requiresDoublePress,
                        float window)
{
    keyBindings[action] = {key, requiresDoublePress, window};
    actionStates[action] = InputActionState::makeDefault();
}

bool InputManager::isDown(Action action) const
{
    auto it = keyBindings.find(action);
    return it != keyBindings.end() &&
           sf::Keyboard::isKeyPressed(it->second.key);
}

bool InputManager::isPressed(Action action, sf::Keyboard::Key pressedKey)
{
    auto it = keyBindings.find(action);
    if (it == keyBindings.end())
    {
        return false;
    }

    const InputBinding &binding = it->second;
    auto &state = actionStates[action];

    if (binding.key == pressedKey)
    {
        if (binding.requiresDoublePress)
        {
            if (state.primed &&
                state.clock.getElapsedTime().asSeconds() < binding.doublePressWindow)
            {
                state.primed = false;
                return true;
            }
            else
            {
                // Prime the double press
                state.primed = true;
                state.clock.restart();
                return false;
            }
        }
        else
        {
            return true;
        }
    }

    return false;
}

std::string InputManager::getKeyName(Action action) const
{
    auto it = keyBindings.find(action);

    if (it == keyBindings.end())
    {
        return "?";
    }

    return keyToString(it->second.key);
}
