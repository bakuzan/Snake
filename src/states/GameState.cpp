#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>

#include "utils/InputUtils.h"
#include "core/SettingsManager.h"
#include "constants/AudioId.h"
#include "constants/Constants.h"
#include "constants/GameMode.h"
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

    initGrid();
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
    if (handleTimers(deltaTime))
    {
        return;
    }

    tickAccumulator += deltaTime;
    while (tickAccumulator >= tickRate)
    {
        tickAccumulator -= tickRate;

        snake.update();

        if (handleGhostSolidification())
        {
            return;
        }

        handleItemConsumption();

        if (handleBoundariesAndPortals())
        {
            return;
        }

        if (checkEndTickDeathConditions())
        {
            return;
        }
    }

    uiManager.update();
}

void GameState::incrementTimeRemaining(float increment)
{
    timeRemaining += increment;

    if (timeRemaining > Constants::TIME_ATTACK_MAX_TIME)
    {
        timeRemaining = Constants::TIME_ATTACK_MAX_TIME;
    }
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

bool GameState::handleTimers(sf::Time deltaTime)
{
    if (gameData.getGameMode() == GameMode::TIME_ATTACK)
    {
        timeRemaining -= deltaTime.asSeconds();
        sf::Color timerColor = (timeRemaining <= 10.0f)
                                   ? sf::Color::Red
                                   : sf::Color::White;

        uiManager.setTime(static_cast<int>(timeRemaining), timerColor);

        if (timeRemaining <= 0.0f)
        {
            timeRemaining = 0.0f;
            onPlayerDeath();
            return true;
        }
    }
    else
    {
        gameTimeSeconds += deltaTime.asSeconds();
        uiManager.setTime(static_cast<int>(gameTimeSeconds));
    }
    return false;
}

bool GameState::handleGhostSolidification()
{
    bool justSolidified = false;
    if (ghostTicksRemaining > 0)
    {
        ghostTicksRemaining--;
        if (ghostTicksRemaining == 0)
        {
            justSolidified = true;
        }
    }

    if (justSolidified &&
        gameData.settingsManager.strictGhostTelefragEnabled)
    {
        bool telefragged = false;
        const auto &segments = snake.getSegments();

        // Check if any body segment materialized inside a wall/hole
        for (const auto &segment : segments)
        {
            if (std::find(holes.begin(), holes.end(), segment) != holes.end())
            {
                telefragged = true;
                break;
            }
        }

        // Check to see if any snake segments overlap with each other
        if (!telefragged)
        {
            std::unordered_set<uint64_t> seenSegments;

            for (const auto &segment : segments)
            {
                // Pack (x, y) into a 64-bit integer for fast set lookup
                uint64_t packedPos = (static_cast<uint64_t>(segment.x) << 32) | static_cast<uint32_t>(segment.y);

                if (!seenSegments.insert(packedPos).second)
                {
                    telefragged = true;
                    break;
                }
            }
        }

        if (telefragged)
        {
            onPlayerDeath();
            return true;
        }
    }

    return false;
}

void GameState::handleItemConsumption()
{
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

        if (gameData.getGameMode() == GameMode::TIME_ATTACK)
        {
            incrementTimeRemaining(Constants::TIME_ATTACK_INCREMENT);
        }

        food.respawn(gridBounds, snake.getSegments(), holes);
        currentScore += Constants::FRUIT_POINT_VALUE;
        uiManager.setScore(currentScore);
    }

    // Special Food
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

                if (gameData.getGameMode() == GameMode::TIME_ATTACK)
                {
                    incrementTimeRemaining(Constants::TIME_ATTACK_INCREMENT * 2);
                }
            }
            else if (currentSpecialFruitType == SpecialFruitType::Poison)
            {
                currentScore = std::max(0, currentScore - Constants::FRUIT_POINT_VALUE * 2);
                snake.grow();

                if (gameData.getGameMode() == GameMode::TIME_ATTACK)
                {
                    incrementTimeRemaining(-Constants::TIME_ATTACK_INCREMENT);
                }

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
}

bool GameState::handleBoundariesAndPortals()
{
    if (gameData.settingsManager.wrapAroundEnabled)
    {
        sf::Vector2i head = snake.getHeadPosition();
        sf::Vector2i wrappedHead;
        wrappedHead.x = (head.x % gridBounds.x + gridBounds.x) % gridBounds.x;
        wrappedHead.y = (head.y % gridBounds.y + gridBounds.y) % gridBounds.y;

        if (wrappedHead != head)
        {
            snake.setHeadPosition(wrappedHead);
        }
    }
    else
    {
        if (checkWallCollision())
        {
            onPlayerDeath();
            return true;
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

    return false;
}

bool GameState::checkEndTickDeathConditions()
{
    if (ghostTicksRemaining <= 0)
    {
        if (isHolesEnabled())
        {
            sf::Vector2i head = snake.getHeadPosition();
            for (const auto &hole : holes)
            {
                if (head == hole)
                {
                    onPlayerDeath();
                    return true;
                }
            }
        }

        if (snake.checkSelfCollision())
        {
            onPlayerDeath();
            return true;
        }
    }
    return false;
}

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

void GameState::initGrid()
{
    float boardWidth = gridBounds.x * Constants::CELL_SIZE;
    float boardHeight = gridBounds.y * Constants::CELL_SIZE;

    // Setup the solid background
    boardBackground.setSize(sf::Vector2f(boardWidth, boardHeight));
    boardBackground.setFillColor(Constants::cellColour);

    // Setup the grid lines
    gridLines.setPrimitiveType(sf::Lines);
    gridLines.resize((gridBounds.x + 1 + gridBounds.y + 1) * 2);

    int vertexIndex = 0;

    // Vertical lines
    for (int x = 0; x <= gridBounds.x; ++x)
    {
        float xPos = x * Constants::CELL_SIZE;
        gridLines[vertexIndex].position = sf::Vector2f(xPos, 0.f);
        gridLines[vertexIndex++].color = Constants::cellBorder;

        gridLines[vertexIndex].position = sf::Vector2f(xPos, boardHeight);
        gridLines[vertexIndex++].color = Constants::cellBorder;
    }

    // Horizontal lines
    for (int y = 0; y <= gridBounds.y; ++y)
    {
        float yPos = y * Constants::CELL_SIZE;
        gridLines[vertexIndex].position = sf::Vector2f(0.f, yPos);
        gridLines[vertexIndex++].color = Constants::cellBorder;

        gridLines[vertexIndex].position = sf::Vector2f(boardWidth, yPos);
        gridLines[vertexIndex++].color = Constants::cellBorder;
    }
}

void GameState::renderGrid()
{
    window.draw(boardBackground);
    window.draw(gridLines);
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
    status = GameStatus::GAME_OVER;
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

        // Check if the cell is free of the snake, other holes, and the head's safe zone
        if (!isCellOccupied(newHole, true))
        {
            // Also ensure it doesn't land on any food or portals
            bool onFood = (newHole == food.getPosition());
            bool onSpecial = (isSpecialFoodActive && newHole == specialFood.getPosition());
            bool onPortal = (gameData.settingsManager.portalsEnabled &&
                             (newHole == portal1 || newHole == portal2));

            if (!onFood && !onSpecial && !onPortal)
            {
                validPosition = true;
            }
        }
    }

    holes.push_back(newHole);
}

void GameState::spawnPortals()
{
    auto getValidPosition = [this]() -> sf::Vector2i
    {
        sf::Vector2i pos;
        bool valid = false;

        while (!valid)
        {
            pos.x = std::rand() % gridBounds.x;
            pos.y = std::rand() % gridBounds.y;

            // Check if cell is free of snake and holes
            if (!isCellOccupied(pos, false))
            {
                bool onFood = (pos == food.getPosition());
                bool onSpecial = (isSpecialFoodActive && pos == specialFood.getPosition());

                if (!onFood && !onSpecial)
                {
                    valid = true;
                }
            }
        }
        return pos;
    };

    portal1 = getValidPosition();
    portal2 = getValidPosition();

    auto getManhattanDistance = [](sf::Vector2i a, sf::Vector2i b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    };

    // Ensure portals aren't spawned right next to each other
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