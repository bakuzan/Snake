#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>
#include <optional>
#include <unordered_map>

#include "core/GameData.h"
#include "core/State.h"
#include "core/StateManager.h"
#include "core/InputManager.h"

#include "components/UIManager.h"

#include "constants/GameStatus.h"

#include "data/SaveData.h"

class GameState : public State
{
private:
    GameData &gameData;
    StateManager &stateManager;
    sf::RenderWindow &window;

    GameStatus status;

    UIManager uiManager;
    InputManager inputManager;

public:
    GameState(GameData &data, StateManager &manager, sf::RenderWindow &win,
              SaveData saveData);
    ~GameState();

    void handleEvent(const sf::Event &event) override;
    void handleWindowResize(sf::Vector2u newSize) override;
    void update(sf::Time deltaTime) override;
    void render() override;

    void handleSystemEvents(const sf::Event &event);
};

#endif // GAMESTATE_H
