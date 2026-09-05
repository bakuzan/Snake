#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SFML/Graphics.hpp>

#include "core/GameData.h"

class UIManager
{
private:
    sf::RenderWindow *window;
    sf::View uiView;
    const GameData &gameData;

    // Elements
    sf::Text scoreText;
    sf::Text timerText;
    sf::Text modeText;

public:
    UIManager(sf::RenderWindow *gameWindow, const GameData &data);
    ~UIManager();

    void handleEvent(sf::Event event);
    void handleResize(unsigned int windowWidth, unsigned int windowHeight);
    void update();
    void render();

    // Access
    void setScore(int newScore);
    void setTime(int totalSeconds);
};

#endif // UIMANAGER_H
