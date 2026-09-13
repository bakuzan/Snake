#include <format>
#include <iostream>

#include "utils/InputUtils.h"
#include "core/SettingsManager.h"
#include "constants/AudioId.h"
#include "constants/Constants.h"
#include "components/ObstacleGenerator.h"
#include "data/GameOverStateConfig.h"

#include "GameState.h"
#include "GameMenuState.h"
#include "GameOverState.h"

GameState::GameState(GameData &data, StateManager &manager, sf::RenderWindow &win,
                     SaveData saveData)
    : gameData(data),
      stateManager(manager),
      window(win),
      status(GameStatus::LOADING),
      uiManager(&window, data),
      // Props
      snake(Constants::CELL_SIZE, {gridBounds.x / 2, gridBounds.y / 2}),
      food(Constants::CELL_SIZE),
      specialFood(Constants::CELL_SIZE)
{
    // Setup Input Manager
    inputManager.bind(Action::PAUSE, sf::Keyboard::Escape);
    inputManager.bind(Action::SAVE, sf::Keyboard::Enter);

    inputManager.bind(Action::MOVE_LEFT, sf::Keyboard::A);
    inputManager.bind(Action::MOVE_RIGHT, sf::Keyboard::D);
    inputManager.bind(Action::MOVE_UP, sf::Keyboard::W);
    inputManager.bind(Action::MOVE_DOWN, sf::Keyboard::S);

    // Setup entities
    if (gameData.settingsManager.obstaclesEnabled)
    {
        sf::Vector2i startPos = ObstacleGenerator::generateLayout(holes, gridBounds);
        snake = Snake(Constants::CELL_SIZE, startPos);
    }

    food.respawn(gridBounds, snake.getSegments(), holes);

    if (gameData.settingsManager.portalsEnabled)
    {
        spawnPortals();
    }

    updateView();

    // Start
    status = GameStatus::PLAYING;
}

GameState::~GameState()
{
    // Destructor
}

// Publics

void GameState::handleEvent(const sf::Event &event)
{
    handleSystemEvents(event);
    handlePlayerEvents(event);

    uiManager.handleEvent(event);
}

void GameState::handleWindowResize(sf::Vector2u newSize)
{
    uiManager.handleResize(newSize.x, newSize.y);
    updateView();
}

void GameState::update(sf::Time deltaTime)
{
    gameTimeSeconds += deltaTime.asSeconds();
    uiManager.setTime(static_cast<int>(gameTimeSeconds));

    tickAccumulator += deltaTime;
    while (tickAccumulator >= tickRate)
    {
        tickAccumulator -= tickRate;

        snake.update();

        if (ghostTicksRemaining > 0)
        {
            ghostTicksRemaining--;
        }

        if (snake.getHeadPosition() == food.getPosition())
        {
            snake.grow();

            if (gameData.settingsManager.holesEnabled)
            {
                spawnSingleHole();
            }

            if (gameData.settingsManager.speedUpEnabled)
            {
                float currentSpeed = tickRate.asSeconds();

                if (currentSpeed > Constants::MAX_SPEED_LIMIT)
                {
                    tickRate = sf::seconds(std::max(Constants::MAX_SPEED_LIMIT, currentSpeed - 0.005f));
                }
            }

            food.respawn(gridBounds, snake.getSegments(), holes);

            currentScore += Constants::FRUIT_POINT_VALUE;
            uiManager.setScore(currentScore);
        }

        if (gameData.settingsManager.specialFoodEnabled)
        {
            if (!isSpecialFoodActive)
            {
                specialFoodSpawnCountdown--;

                if (specialFoodSpawnCountdown <= 0)
                {
                    updateSpecialFruitType();

                    specialFood.respawn(gridBounds, snake.getSegments(), holes);
                    isSpecialFoodActive = true;
                    specialFoodDuration = Constants::SPECIAL_FRUIT_DURATION;
                }
            }
            else
            {
                specialFoodDuration--;

                if (specialFoodDuration <= 0)
                {
                    isSpecialFoodActive = false;
                    specialFoodSpawnCountdown = Constants::SPECIAL_FRUIT_SPAWN_COUNTDOWN;
                }
            }

            if (isSpecialFoodActive &&
                snake.getHeadPosition() == specialFood.getPosition())
            {
                if (currentSpecialFruitType == SpecialFruitType::Golden)
                {
                    currentScore += Constants::FRUIT_POINT_VALUE * 5;
                }
                else if (currentSpecialFruitType == SpecialFruitType::Poison)
                {
                    currentScore = std::max(0, currentScore - Constants::FRUIT_POINT_VALUE * 2);
                    snake.grow();
                    if (gameData.settingsManager.holesEnabled)
                    {
                        spawnSingleHole();
                    }
                }
                else if (currentSpecialFruitType == SpecialFruitType::Ghost)
                {
                    ghostTicksRemaining = Constants::GHOST_TICKS_DURATION;
                }

                uiManager.setScore(currentScore);
                isSpecialFoodActive = false;
                specialFoodSpawnCountdown = Constants::SPECIAL_FRUIT_SPAWN_COUNTDOWN;
            }
        }

        if (gameData.settingsManager.wrapAroundEnabled)
        {
            sf::Vector2i head = snake.getHeadPosition();
            bool wrapped = false;

            // Check horizontal bounds
            if (head.x < 0)
            {
                head.x = gridBounds.x - 1;
                wrapped = true;
            }
            else if (head.x >= gridBounds.x)
            {
                head.x = 0;
                wrapped = true;
            }

            // Check vertical bounds
            if (head.y < 0)
            {
                head.y = gridBounds.y - 1;
                wrapped = true;
            }
            else if (head.y >= gridBounds.y)
            {
                head.y = 0;
                wrapped = true;
            }

            if (wrapped)
            {
                snake.setHeadPosition(head);
            }
        }
        else
        {
            if (checkWallCollision())
            {
                status = GameStatus::GAME_OVER;
                onPlayerDeath();
                return;
            }
        }

        if (gameData.settingsManager.portalsEnabled)
        {
            sf::Vector2i head = snake.getHeadPosition();

            if (head == portal1)
            {
                snake.setHeadPosition(portal2);
            }
            else if (head == portal2)
            {
                snake.setHeadPosition(portal1);
            }
        }

        if (ghostTicksRemaining <= 0)
        {
            if (isHolesEnabled())
            {
                sf::Vector2i head = snake.getHeadPosition();

                for (const auto &hole : holes)
                {
                    if (head == hole)
                    {
                        status = GameStatus::GAME_OVER;
                        onPlayerDeath();
                        return;
                    }
                }
            }

            if (snake.checkSelfCollision())
            {
                status = GameStatus::GAME_OVER;
                onPlayerDeath();
                return;
            }
        }
    }

    uiManager.update();
}

void GameState::render()
{
    if (status == GameStatus::LOADING)
    {
        return;
    }

    // ---- MAIN GAME
    window.clear();
    window.setView(gameView);

    renderGrid();

    if (isHolesEnabled())
    {
        sf::RectangleShape holeShape(sf::Vector2f(Constants::CELL_SIZE, Constants::CELL_SIZE));
        holeShape.setFillColor(Constants::pitColour);
        holeShape.setOutlineThickness(-1.f);
        holeShape.setOutlineColor(Constants::pitBorder);

        for (const auto &hole : holes)
        {
            holeShape.setPosition(hole.x * Constants::CELL_SIZE,
                                  hole.y * Constants::CELL_SIZE);

            window.draw(holeShape);
        }
    }

    if (gameData.settingsManager.portalsEnabled)
    {
        renderPortal(portal1, Constants::portal1Colour);
        renderPortal(portal2, Constants::portal2Colour);
    }

    snake.render(window, ghostTicksRemaining);
    food.render(window);

    if (gameData.settingsManager.specialFoodEnabled &&
        isSpecialFoodActive)
    {
        renderSpecialFruit();
    }

    // ---- UI Elements
    uiManager.render();
}

// Privates

bool GameState::checkWallCollision() const
{
    sf::Vector2i head = snake.getHeadPosition();
    return head.x < 0 ||
           head.x >= gridBounds.x ||
           head.y < 0 ||
           head.y >= gridBounds.y;
}

void GameState::handleSystemEvents(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed &&
        inputManager.isPressed(Action::PAUSE, event.key.code))
    {
        stateManager.pushState(std::make_unique<GameMenuState>(gameData, stateManager, window));
    }
}

void GameState::handlePlayerEvents(const sf::Event &event)
{
    if (event.type != sf::Event::KeyPressed)
    {
        return;
    }

    if (inputManager.isPressed(Action::MOVE_UP, event.key.code))
    {
        snake.handleInput(Direction::UP);
    }
    if (inputManager.isPressed(Action::MOVE_DOWN, event.key.code))
    {
        snake.handleInput(Direction::DOWN);
    }
    if (inputManager.isPressed(Action::MOVE_LEFT, event.key.code))
    {
        snake.handleInput(Direction::LEFT);
    }
    if (inputManager.isPressed(Action::MOVE_RIGHT, event.key.code))
    {
        snake.handleInput(Direction::RIGHT);
    }
}

void GameState::updateView()
{
    sf::Vector2f winSize(static_cast<float>(window.getSize().x),
                         static_cast<float>(window.getSize().y));
    gameView.setSize(winSize);

    float boardWidth = gridBounds.x * Constants::CELL_SIZE;
    float boardHeight = gridBounds.y * Constants::CELL_SIZE;

    sf::Vector2f boardCenter(boardWidth / 2.f, boardHeight / 2.f);
    boardCenter.y -= 40.f; // Need space for UI at top

    gameView.setCenter(boardCenter);
}

void GameState::renderGrid()
{
    sf::RectangleShape cell(sf::Vector2f(Constants::CELL_SIZE, Constants::CELL_SIZE));
    cell.setFillColor(Constants::cellColour);

    cell.setOutlineThickness(-1.f);
    cell.setOutlineColor(Constants::cellBorder);

    for (int x = 0; x < gridBounds.x; ++x)
    {
        for (int y = 0; y < gridBounds.y; ++y)
        {
            cell.setPosition(x * Constants::CELL_SIZE,
                             y * Constants::CELL_SIZE);

            window.draw(cell);
        }
    }
}

void GameState::renderSpecialFruit()
{
    sf::Color specialColour;

    switch (currentSpecialFruitType)
    {
    case SpecialFruitType::Golden:
        specialColour = Constants::bonusFruitColour;
        break;
    case SpecialFruitType::Poison:
        specialColour = Constants::poisonFruitColour;
        break;
    case SpecialFruitType::Ghost:
        specialColour = Constants::ghostFruitColour;
        break;
    }

    specialFood.render(window, specialColour);
}

void GameState::renderPortal(const sf::Vector2i &portal, sf::Color portalColour)
{
    sf::RectangleShape portalShape(sf::Vector2f(Constants::CELL_SIZE, Constants::CELL_SIZE));
    portalShape.setPosition(portal.x * Constants::CELL_SIZE,
                            portal.y * Constants::CELL_SIZE);
    portalShape.setFillColor(portalColour);
    window.draw(portalShape);
}

void GameState::onPlayerDeath()
{
    GameOverStateConfig config = GameOverStateConfig::defaultValues(currentScore);
    stateManager.pushState(std::make_unique<GameOverState>(gameData, stateManager, window, config));
}

void GameState::spawnSingleHole()
{
    sf::Vector2i newHole;
    bool validPosition = false;

    while (!validPosition)
    {
        newHole.x = std::rand() % gridBounds.x;
        newHole.y = std::rand() % gridBounds.y;
        validPosition = true;

        for (const auto &segment : snake.getSegments())
        {
            if (newHole == segment)
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
            if (newHole == hole)
            {
                validPosition = false;
                break;
            }
        }

        if (!validPosition)
        {
            continue;
        }

        if (newHole == food.getPosition())
        {
            validPosition = false;
            continue;
        }

        sf::Vector2i head = snake.getHeadPosition();
        if (std::abs(newHole.x - head.x) <= 2 &&
            std::abs(newHole.y - head.y) <= 2)
        {
            validPosition = false;
            continue;
        }
    }

    holes.push_back(newHole);
}

void GameState::spawnPortals()
{
    auto getValidPosition = [this]() -> sf::Vector2i
    {
        sf::Vector2i pos;
        bool valid;
        do
        {
            valid = true;
            pos.x = std::rand() % gridBounds.x;
            pos.y = std::rand() % gridBounds.y;

            // Ensure it doesn't spawn on the snake
            for (const auto &segment : snake.getSegments())
            {
                if (pos == segment)
                {
                    valid = false;
                }
            }

            // Ensure it doesn't spawn on holes
            for (const auto &hole : holes)
            {
                if (pos == hole)
                {
                    valid = false;
                }
            }

            if (pos == food.getPosition())
            {
                valid = false;
            }

            if (isSpecialFoodActive &&
                pos == specialFood.getPosition())
            {
                valid = false;
            }

        } while (!valid);
        return pos;
    };

    portal1 = getValidPosition();
    portal2 = getValidPosition();

    auto getManhattanDistance = [](sf::Vector2i a, sf::Vector2i b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    };

    while (getManhattanDistance(portal1, portal2) < Constants::MIN_PORTAL_DISTANCE)
    {
        portal2 = getValidPosition();
    }
}

void GameState::generateHoles(int count)
{
    holes.clear();

    for (int i = 0; i < count; ++i)
    {
        sf::Vector2i newHole;
        bool validPosition = false;

        while (!validPosition)
        {
            newHole.x = std::rand() % gridBounds.x;
            newHole.y = std::rand() % gridBounds.y;

            if (!isCellOccupied(newHole, true))
            {
                validPosition = true;
                holes.push_back(newHole);
            }
        }
    }
}

void GameState::updateSpecialFruitType()
{
    int roll = std::rand() % 100;

    if (snake.getSegments().size() > 15 && roll < 15) // 15% chance if long enough
    {
        currentSpecialFruitType = SpecialFruitType::Ghost;
    }
    else if (roll < 75) // ~60% Golden
    {
        currentSpecialFruitType = SpecialFruitType::Golden;
    }
    else // ~25% Poison
    {
        currentSpecialFruitType = SpecialFruitType::Poison;
    }
}

bool GameState::isCellOccupied(sf::Vector2i cell, bool includeSafeZone) const
{
    for (const auto &segment : snake.getSegments())
    {
        if (cell == segment)
        {
            return true;
        }
    }

    for (const auto &hole : holes)
    {
        if (cell == hole)
        {
            return true;
        }
    }

    if (includeSafeZone)
    {
        sf::Vector2i startPos = snake.getHeadPosition();

        // 3x3 exclusion zone
        if (std::abs(cell.x - startPos.x) <= 3 &&
            std::abs(cell.y - startPos.y) <= 3)
        {
            return true;
        }
    }

    return false;
}

bool GameState::isHolesEnabled()
{
    return gameData.settingsManager.holesEnabled ||
           gameData.settingsManager.obstaclesEnabled;
}