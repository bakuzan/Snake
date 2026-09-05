#include <format>

#include "constants/Constants.h"
#include "UIManager.h"

UIManager::UIManager(sf::RenderWindow *gameWindow, const GameData &data)
    : window(gameWindow),
      gameData(data)
{
    // Setup Texts
    scoreText.setFont(gameData.gameFont);
    scoreText.setCharacterSize(28);
    scoreText.setFillColor(sf::Color::White);
    setScore(0);

    timerText.setFont(gameData.gameFont);
    timerText.setCharacterSize(28);
    timerText.setFillColor(sf::Color::White);
    setTime(0);

    modeText.setFont(gameData.gameFont);
    modeText.setCharacterSize(28);
    modeText.setFillColor(sf::Color::Yellow);
    modeText.setString(gameData.settingsManager.getModeName());

    sf::Vector2u windowSize = window->getSize();
    handleResize(windowSize.x, windowSize.y);
}

UIManager::~UIManager()
{
    // Destructor
}

// Publics

void UIManager::handleEvent(sf::Event event)
{
    (void)event;

    sf::View prevView = window->getView();
    window->setView(uiView); // Switch to UI view

    // Make changes

    window->setView(prevView); // Restore previous view
}

void UIManager::handleResize(unsigned int windowWidth, unsigned int windowHeight)
{
    uiView.reset(sf::FloatRect(0.f, 0.f,
                               static_cast<float>(windowWidth),
                               static_cast<float>(windowHeight)));

    float boardWidth = Constants::GRID_WIDTH * Constants::CELL_SIZE;
    float startX = (windowWidth - boardWidth) / 2.f;
    float yPos = 20.f;

    // Set label positions
    scoreText.setPosition(startX, yPos);

    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    modeText.setPosition(startX, (yPos * 2) + scoreBounds.height);

    sf::FloatRect timerBounds = timerText.getLocalBounds();
    timerText.setPosition(startX + boardWidth - timerBounds.width, yPos);
}

void UIManager::update()
{
}

void UIManager::render()
{
    sf::View prevView = window->getView();
    window->setView(uiView); // Switch to UI view

    // UI ELEMENTS HERE
    window->draw(scoreText);
    window->draw(timerText);
    window->draw(modeText);

    window->setView(prevView); // Restore previous view
}

void UIManager::setScore(int newScore)
{
    scoreText.setString(std::format("{:06}", newScore));
}

void UIManager::setTime(int totalSeconds)
{
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    timerText.setString(std::format("{:02}:{:02}", minutes, seconds));
}
