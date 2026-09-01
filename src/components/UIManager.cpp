#include "UIManager.h"

UIManager::UIManager(sf::RenderWindow *gameWindow, const GameData &data)
    : window(gameWindow),
      gameData(data)
{
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
}

void UIManager::update()
{
}

void UIManager::render()
{
    sf::View prevView = window->getView();
    window->setView(uiView); // Switch to UI view

    // UI ELEMENTS HERE

    window->setView(prevView); // Restore previous view
}
