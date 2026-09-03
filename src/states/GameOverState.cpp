#include <memory>
#include <format>

#include "constants/Constants.h"
#include "core/SettingsManager.h"
#include "data/SaveData.h"
#include "utils/InputUtils.h"
#include "utils/GameUtils.h"
#include "utils/DataUtils.h"
#include "MainMenuState.h"
#include "GameOverState.h"
#include "GameState.h"

GameOverState::GameOverState(GameData &data, StateManager &manager, sf::RenderWindow &win,
                             GameOverStateConfig config)
    : gameData(data), stateManager(manager), window(win),
      stateConfig(config)
{
    buttonSpacing = 20.f;
    sf::Vector2f viewSize = gameOverView.getSize();
    sf::Vector2f center(gameOverView.getCenter());
    window.setView(gameOverView);

    // Configure background
    background.setSize(sf::Vector2f(viewSize.x - 20.0f, viewSize.y - 20.0f));
    background.setFillColor(sf::Color(0, 0, 0, 150));

    // Configure text
    gameOverText.setFont(gameData.gameFont);
    gameOverText.setString(stateConfig.titleText);
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(stateConfig.titleTextColour);

    // Add buttons
    addButton("New", "New Game",
              [this]()
              {  gameData.reset(); 
                stateManager.changeState(std::make_unique<GameState>(gameData, stateManager, window, SaveData::makeDefault())); });
    addButton("Menu", "Main Menu",
              [this]()
              {
                  gameData.reset();
                  stateManager.replaceStates(std::make_unique<MainMenuState>(gameData, stateManager, window));
              });
    addButton("Exit", "Exit",
              [this]()
              { window.close(); });

    // To ensure positioning is updated relative to window resizing
    updateMenuItemPositions();
}

GameOverState::~GameOverState()
{
    // Destructor
}

// Publics

void GameOverState::handleEvent(const sf::Event &event)
{
    InputUtils::handleButtonEvent(event, buttons, window, selectedButtonIndex);
}

void GameOverState::handleWindowResize(sf::Vector2u newSize)
{
    (void)newSize;
    updateMenuItemPositions();
}

void GameOverState::update(sf::Time deltaTime)
{
    (void)deltaTime;

    gameData.audioManager.cleanupSounds();

    // if (gameData.audioManager.getSoundStatus(AudioId::AMBIENT) == sf::Sound::Status::Playing)
    // {
    //     gameData.audioManager.stopSound(AudioId::AMBIENT);
    // }
}

void GameOverState::render()
{
    window.setView(gameOverView);
    window.draw(background);
    window.draw(gameOverText);

    for (const auto &button : buttons)
    {
        button.render(window);
    }
}

// Privates

void GameOverState::addButton(std::string id,
                              std::string label,
                              std::function<void()> callback)
{
    buttons.emplace_back(id, gameData.gameFont, label, sf::Vector2f(0.f, 0.f), callback);
}

void GameOverState::updateMenuItemPositions()
{
    sf::Vector2f viewCenter = gameOverView.getCenter();
    sf::Vector2f viewSize = gameOverView.getSize();
    sf::Vector2f backgroundSize = background.getSize();

    background.setPosition(
        viewCenter.x - backgroundSize.x / 2.f,
        viewCenter.y - backgroundSize.y / 2.f);

    float topEdge = viewCenter.y - viewSize.y / 2.0f;

    gameOverText.setPosition(
        viewCenter.x - (gameOverText.getGlobalBounds().width / 2.0f),
        topEdge + buttonSpacing);

    if (buttons.empty())
    {
        return;
    }

    float buttonRowY = viewCenter.y + viewSize.y / 2.f - Constants::BUTTON_HEIGHT - buttonSpacing;
    float startX = viewCenter.x - viewSize.x / 2.f;

    for (size_t i = 0; i < buttons.size(); ++i)
    {
        float buttonWidthOffset = i * Constants::BUTTON_WIDTH;
        buttons[i].setPosition(sf::Vector2f(startX + (i * buttonSpacing) + buttonWidthOffset, buttonRowY));
    }
}
