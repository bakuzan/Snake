#include <memory>
#include <format>

#include "constants/Constants.h"
#include "core/SettingsManager.h"
#include "utils/InputUtils.h"
#include "utils/GameUtils.h"
#include "utils/DataUtils.h"
#include "MainMenuState.h"
#include "GameOverState.h"

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
    float buttonRowY = center.y + viewSize.y / 2.f - Constants::BUTTON_HEIGHT - buttonSpacing;

    buttons.emplace_back("Menu", gameData.gameFont, "Main Menu",
                         sf::Vector2f(center.x - viewSize.x / 2.f + (buttonSpacing * 2.0f) + Constants::BUTTON_WIDTH, buttonRowY),
                         [this]()
                         {
                             /* gameData.reset(); */
                             stateManager.replaceStates(std::make_unique<MainMenuState>(gameData, stateManager, window));
                         });
    buttons.emplace_back("Exit", gameData.gameFont, "Exit",
                         sf::Vector2f(center.x + viewSize.x / 2.f - Constants::BUTTON_WIDTH - buttonSpacing, buttonRowY),
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

    float buttonRowY = viewCenter.y + viewSize.y / 2.f - Constants::BUTTON_HEIGHT - buttonSpacing;
    buttons[0].setPosition(sf::Vector2f(viewCenter.x - viewSize.x / 2.f + buttonSpacing, buttonRowY));
    buttons[1].setPosition(sf::Vector2f(viewCenter.x - viewSize.x / 2.f + (buttonSpacing * 2.0f) + Constants::BUTTON_WIDTH, buttonRowY));
    buttons[2].setPosition(sf::Vector2f(viewCenter.x + viewSize.x / 2.f - Constants::BUTTON_WIDTH - buttonSpacing, buttonRowY));
}
