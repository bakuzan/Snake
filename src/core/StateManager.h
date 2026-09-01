#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <memory>
#include <stack>

#include "State.h"
#include "constants/PendingChange.h"

class StateManager
{
private:
    std::vector<std::unique_ptr<State>> states;
    std::unique_ptr<State> pendingState;
    PendingChange pendingChange;

private:
    void applyPendingStateChange();

public:
    StateManager();
    ~StateManager();

    void handleEvent(const sf::Event &event);
    void update(sf::Time deltaTime);
    void render();

    void handleWindowResize(sf::Vector2u newSize);

    // Helpers
    void pushState(std::unique_ptr<State> state);
    void popState();
    void changeState(std::unique_ptr<State> state);
    void replaceStates(std::unique_ptr<State> state);
};

#endif // STATEMANAGER_H
