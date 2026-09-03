#include <format>
#include <iostream>

#include "utils/InputUtils.h"
#include "core/SettingsManager.h"
#include "constants/AudioId.h"
#include "constants/Constants.h"
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
      snake(Constants::CELL_SIZE, {15, 10}),
      food(Constants::CELL_SIZE)
{
    // Setup Input Manager
    inputManager.bind(Action::PAUSE, sf::Keyboard::Escape);
    inputManager.bind(Action::SAVE, sf::Keyboard::Enter);

    inputManager.bind(Action::MOVE_LEFT, sf::Keyboard::A);
    inputManager.bind(Action::MOVE_RIGHT, sf::Keyboard::D);
    inputManager.bind(Action::MOVE_UP, sf::Keyboard::W);
    inputManager.bind(Action::MOVE_DOWN, sf::Keyboard::S);

    // Setup entities
    food.respawn(gridBounds, snake.getSegments());

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

        if (snake.getHeadPosition() == food.getPosition())
        {
            snake.grow();
            food.respawn(gridBounds, snake.getSegments());

            currentScore += 100;
            uiManager.setScore(currentScore);
        }

        if (checkWallCollision() ||
            snake.checkSelfCollision())
        {
            status = GameStatus::GAME_OVER;
            onPlayerDeath();
            return;
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
    snake.render(window);
    food.render(window);

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

void GameState::onPlayerDeath()
{
    GameOverStateConfig config = GameOverStateConfig::defaultValues(currentScore);
    stateManager.pushState(std::make_unique<GameOverState>(gameData, stateManager, window, config));
}