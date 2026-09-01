#include <format>
#include <iostream>

#include "utils/GameUtils.h"
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
      uiManager(&window, data)
{
    // Setup Input Manager
    inputManager.bind(Action::PAUSE, sf::Keyboard::Escape);
    inputManager.bind(Action::SAVE, sf::Keyboard::Enter);

    inputManager.bind(Action::MOVE_LEFT, sf::Keyboard::A);
    inputManager.bind(Action::MOVE_RIGHT, sf::Keyboard::D);
    inputManager.bind(Action::MOVE_UP, sf::Keyboard::W);
    inputManager.bind(Action::MOVE_DOWN, sf::Keyboard::S);
}

GameState::~GameState()
{
    // Destructor
}

// Publics

void GameState::handleEvent(const sf::Event &event)
{
    uiManager.handleEvent(event);
}

void GameState::handleWindowResize(sf::Vector2u newSize)
{
    uiManager.handleResize(newSize.x, newSize.y);
}

void GameState::update(sf::Time deltaTime)
{
    // TODO restore when implemented
    // if (gameData.audioManager.getSoundStatus(AudioId::AMBIENT) != sf::Sound::Status::Playing)
    // {
    //     gameData.audioManager.playSound(AudioId::AMBIENT, true);
    // }

    (void)deltaTime;
    // float dt = deltaTime.asSeconds();

    // Read inputs
    // bool leftHeld = inputManager.isDown(Action::MOVE_LEFT);
    // bool rightHeld = inputManager.isDown(Action::MOVE_RIGHT);
    // bool upHeld = inputManager.isDown(Action::MOVE_UP);
    // bool downHeld = inputManager.isDown(Action::MOVE_DOWN);

    // UI handling
    uiManager.update();
}

void GameState::render()
{
    if (status == GameStatus::LOADING)
    {
        return;
    }

    // ---- MAIN GAME
    // TODO

    // ---- UI Elements
    uiManager.render();
}
