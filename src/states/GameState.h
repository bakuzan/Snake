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
#include "constants/Constants.h"
#include "constants/GameStatus.h"
#include "data/SaveData.h"
#include "entities/Food.h"
#include "entities/Snake.h"

class GameState : public State
{
private:
    GameData &gameData;
    StateManager &stateManager;
    sf::RenderWindow &window;
    sf::View gameView;

    GameStatus status;

    UIManager uiManager;
    InputManager inputManager;

    // Game props
    sf::Vector2i gridBounds{Constants::GRID_WIDTH, Constants::GRID_WIDTH};
    sf::Time tickRate{sf::seconds(0.12f)};
    sf::Time tickAccumulator{sf::Time::Zero};

    Snake snake;
    Food food;

    int currentScore{0};
    float gameTimeSeconds{0.f};

public:
    GameState(GameData &data, StateManager &manager, sf::RenderWindow &win,
              SaveData saveData);
    ~GameState();

    void handleEvent(const sf::Event &event) override;
    void handleWindowResize(sf::Vector2u newSize) override;
    void update(sf::Time deltaTime) override;
    void render() override;

private:
    bool checkWallCollision() const;

    void handleSystemEvents(const sf::Event &event);
    void handlePlayerEvents(const sf::Event &event);
    void updateView();
    void renderGrid();
    void onPlayerDeath();
};

#endif // GAMESTATE_H
