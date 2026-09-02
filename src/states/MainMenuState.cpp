#include "constants/Constants.h"
#include "data/SaveData.h"
#include "utils/InputUtils.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "SettingsState.h"

MainMenuState::MainMenuState(GameData &data, StateManager &manager, sf::RenderWindow &win)
    : gameData(data), stateManager(manager), window(win)
{
    buttonSpacing = Constants::BUTTON_HEIGHT + 10.f;
    sf::Vector2f center(menuView.getCenter());
    window.setView(menuView);

    // Setup title
    gameTitle.setFont(gameData.gameFont);
    gameTitle.setString(Constants::PROJECT_NAME);
    gameTitle.setCharacterSize(48);
    gameTitle.setFillColor(sf::Color::White);

    // Add buttons
    addButton("New", "New Game",
              [this]()
              {  gameData.reset(); 
                stateManager.changeState(std::make_unique<GameState>(gameData, stateManager, window, SaveData::makeDefault())); });
    addButton("Settings", "Settings",
              [this]()
              { stateManager.changeState(std::make_unique<SettingsState>(gameData, stateManager, window)); });
    addButton("Exit", "Exit",
              [this]()
              { window.close(); });

    // To ensure positioning is updated relative to window resizing
    updateMenuItemPositions();
}

MainMenuState::~MainMenuState()
{
    // Destructor
}

// Publics

void MainMenuState::handleEvent(const sf::Event &event)
{
    InputUtils::handleButtonEvent(event, buttons, window, selectedButtonIndex);
}

void MainMenuState::handleWindowResize(sf::Vector2u newSize)
{
    (void)newSize;
    updateMenuItemPositions();
}

void MainMenuState::update(sf::Time deltaTime)
{
    (void)deltaTime;

    gameData.audioManager.cleanupSounds();

    // if (gameData.audioManager.getSoundStatus(AudioId::AMBIENT) == sf::Sound::Status::Playing)
    // {
    //     gameData.audioManager.stopSound(AudioId::AMBIENT);
    // }
}

void MainMenuState::render()
{
    window.setView(menuView);
    window.draw(gameTitle);

    for (const auto &button : buttons)
    {
        button.render(window);
    }
}

// Privates

void MainMenuState::addButton(std::string id,
                              std::string label,
                              std::function<void()> callback)
{
    buttons.emplace_back(id, gameData.gameFont, label, sf::Vector2f(0.f, 0.f), callback);
}

void MainMenuState::updateMenuItemPositions()
{
    sf::Vector2f viewCenter = menuView.getCenter();
    sf::Vector2f viewSize = menuView.getSize();

    gameTitle.setPosition(viewCenter.x - viewSize.x / 2.f + 25.f,
                          viewCenter.y - viewSize.y / 2.f + 25.f);

    if (buttons.empty())
    {
        return;
    }

    float offsetX = viewCenter.x - (Constants::BUTTON_WIDTH / 2.0f);
    float totalHeight = (buttons.size() - 1) * buttonSpacing;
    float startY = viewCenter.y - (totalHeight / 2.0f);

    for (size_t i = 0; i < buttons.size(); ++i)
    {
        float yPos = startY + (i * buttonSpacing);
        buttons[i].setPosition(sf::Vector2f(offsetX, yPos));
    }
}
