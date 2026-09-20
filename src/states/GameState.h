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
#include "constants/SpecialFruitType.h"
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
    sf::RectangleShape boardBackground;
    sf::VertexArray gridLines;
    sf::Vector2i gridBounds{Constants::GRID_WIDTH, Constants::GRID_WIDTH};
    sf::Time tickRate{sf::seconds(0.12f)};
    sf::Time tickAccumulator{sf::Time::Zero};

    Snake snake;
    Food food;
    Food specialFood;
    std::vector<sf::Vector2i> holes;

    // Special food things
    bool isSpecialFoodActive{false};
    SpecialFruitType currentSpecialFruitType{SpecialFruitType::Golden};
    int ghostTicksRemaining{0};
    int specialFoodSpawnCountdown{Constants::SPECIAL_FRUIT_SPAWN_COUNTDOWN}; // Wait X snake steps before spawning
    int specialFoodDuration{Constants::SPECIAL_FRUIT_DURATION};              // On screen for X snake steps

    // Portals
    sf::Vector2i portal1;
    sf::Vector2i portal2;

    // Game meta
    int currentScore{0};
    float gameTimeSeconds{0.f};
    float timeRemaining{Constants::TIME_ATTACK_START_TIME}; // Time Attack state

public:
    GameState(GameData &data, StateManager &manager, sf::RenderWindow &win,
              SaveData saveData);
    ~GameState();

    void handleEvent(const sf::Event &event) override;
    void handleWindowResize(sf::Vector2u newSize) override;
    void update(sf::Time deltaTime) override;
    void render() override;

private:
    bool handleTimers(sf::Time deltaTime);
    bool handleGhostSolidification();
    void handleItemConsumption();
    bool handleBoundariesAndPortals();
    bool checkEndTickDeathConditions();

    bool checkWallCollision() const;

    void handleSystemEvents(const sf::Event &event);
    void handlePlayerEvents(const sf::Event &event);
    void updateView();
    void initGrid();
    void renderGrid();
    void renderSpecialFruit();
    void renderPortal(const sf::Vector2i &portal, sf::Color portalColour);
    void onPlayerDeath();
    void spawnSingleHole();
    void spawnPortals();
    void generateHoles(int count);
    void updateSpecialFruitType();
    void incrementTimeRemaining(float increment);
    bool isCellOccupied(sf::Vector2i cell, bool includeSafeZone = false) const;
    bool isHolesEnabled();
};

#endif // GAMESTATE_H
