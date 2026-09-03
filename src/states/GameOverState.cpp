#include <memory>
#include <format>
#include <cctype>

#include "constants/Constants.h"
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
      stateConfig(config),
      isEnteringInitials(false)
{
    buttonSpacing = 20.f;
    sf::Vector2f center(gameOverView.getCenter());
    window.setView(gameOverView);

    // Load existing high scores
    highScoreManager.loadFromFile("highscores.txt");

    // Check if score qualifies for high score list
    if (highScoreManager.isHighScore(stateConfig.score))
    {
        isEnteringInitials = true;
    }

    // Configure background
    sf::Vector2f viewSize = gameOverView.getSize();
    background.setSize(sf::Vector2f(viewSize.x - 20.0f, viewSize.y - 20.0f));
    background.setFillColor(sf::Color(0, 0, 0, 200));

    // Configure text
    gameOverText.setFont(gameData.gameFont);
    gameOverText.setString(stateConfig.titleText);
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(stateConfig.titleTextColour);

    // Initials Input Texts
    initialsPromptText.setFont(gameData.gameFont);
    initialsPromptText.setCharacterSize(22);
    initialsPromptText.setFillColor(sf::Color::White);
    initialsPromptText.setString(std::format("ENTER INITIALS AND PRESS ENTEER:"));

    initialsDisplayText.setFont(gameData.gameFont);
    initialsDisplayText.setCharacterSize(36);
    initialsDisplayText.setFillColor(sf::Color::Yellow);
    initialsDisplayText.setString("[ _ _ _ ]");

    // Leaderboard Display Text
    leaderboardText.setFont(gameData.gameFont);
    leaderboardText.setCharacterSize(28);
    leaderboardText.setFillColor(sf::Color::White);

    buildLeaderboardText();

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
    if (isEnteringInitials)
    {
        if (event.type == sf::Event::TextEntered)
        {
            uint32_t unicode = event.text.unicode;

            // Handle Backspace
            if (unicode == 8 &&
                !playerInitials.empty())
            {
                playerInitials.pop_back();
            }
            // Handle Enter
            else if ((unicode == 13 || unicode == 10) &&
                     playerInitials.length() == 3)
            {
                highScoreManager.addScore(playerInitials, stateConfig.score);
                highScoreManager.saveToFile("highscores.txt");

                isEnteringInitials = false;

                // Reset Title and refresh Leaderboard
                gameOverText.setString(stateConfig.titleText);
                gameOverText.setFillColor(stateConfig.titleTextColour);
                buildLeaderboardText();
                updateMenuItemPositions();
            }
            // Handle Letters A-Z
            else if (playerInitials.length() < 3 &&
                     std::isalpha(static_cast<char>(unicode)))
            {
                playerInitials += static_cast<char>(std::toupper(unicode));
            }

            // Visual feedback
            std::string formattedBox = "[ ";
            for (int i = 0; i < 3; ++i)
            {
                if (i < static_cast<int>(playerInitials.length()))
                {
                    formattedBox += playerInitials[i];
                }
                else
                {
                    formattedBox += '_';
                }

                formattedBox += ' ';
            }

            formattedBox += ']';
            initialsDisplayText.setString(formattedBox);

            // Re-center input text as content updates
            sf::FloatRect bounds = initialsDisplayText.getLocalBounds();
            sf::Vector2f viewCenter = gameOverView.getCenter();
            initialsDisplayText.setPosition(viewCenter.x - (bounds.width / 2.f),
                                            initialsDisplayText.getPosition().y);
        }

        // Block button input execution while entering initials
        return;
    }

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

    if (isEnteringInitials)
    {
        window.draw(initialsPromptText);
        window.draw(initialsDisplayText);
    }
    else
    {
        window.draw(leaderboardText);

        for (const auto &button : buttons)
        {
            button.render(window);
        }
    }
}

// Privates

void GameOverState::buildLeaderboardText()
{
    std::string formattedList = std::format("{:<4}{:<12}{:>6}\n", "POS", "NAME", "SCORE");
    formattedList += "----------------------\n";

    const auto &scores = highScoreManager.getHighScores();

    for (size_t i = 0; i < scores.size(); ++i)
    {
        std::string posStr = std::format("{:02}", i + 1);

        // {:<4}   -> Position string, padded with spaces up to 7 chars
        // {:.<12} -> Name left-aligned, padded with dots up to 12 chars
        // {:06}   -> Score padded with leading zeros (naturally 6 chars wide)
        formattedList += std::format("{:<4}{:.<12}{:06}\n",
                                     posStr,
                                     scores[i].name,
                                     scores[i].score);
    }

    if (scores.empty())
    {
        formattedList += "\n   NO HIGH SCORES YET!";
    }

    leaderboardText.setString(formattedList);
}

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

    // Center Background
    background.setSize(sf::Vector2f(viewSize.x - 60.f, viewSize.y - 60.f));
    background.setPosition(viewCenter.x - background.getSize().x / 2.f,
                           viewCenter.y - background.getSize().y / 2.f);

    // Title Text Position
    sf::FloatRect titleBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition(viewCenter.x - (titleBounds.width / 2.f),
                             viewCenter.y - viewSize.y / 2.f + 50.f);

    // Initials Input Mode Layout
    sf::FloatRect promptBounds = initialsPromptText.getLocalBounds();
    initialsPromptText.setPosition(viewCenter.x - (promptBounds.width / 2.f),
                                   viewCenter.y - 40.f);

    sf::FloatRect displayBounds = initialsDisplayText.getLocalBounds();
    initialsDisplayText.setPosition(viewCenter.x - (displayBounds.width / 2.f),
                                    viewCenter.y + 40.f);

    // High Score Table Layout
    sf::FloatRect boardBounds = leaderboardText.getLocalBounds();
    leaderboardText.setPosition(viewCenter.x - (boardBounds.width / 2.f),
                                viewCenter.y - viewSize.y / 2.f + 150.f);

    if (buttons.empty())
    {
        return;
    }

    float buttonRowY = viewCenter.y + viewSize.y / 2.f - Constants::BUTTON_HEIGHT - 50.f;
    float totalWidth = (buttons.size() * Constants::BUTTON_WIDTH) + ((buttons.size() - 1) * buttonSpacing);
    float startX = viewCenter.x - (totalWidth / 2.f);

    for (size_t i = 0; i < buttons.size(); ++i)
    {
        buttons[i].setPosition(sf::Vector2f(startX + i * (Constants::BUTTON_WIDTH + buttonSpacing), buttonRowY));
    }
}
