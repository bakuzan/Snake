#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include <string>

#include "ui/Button.h"
#include "core/GameData.h"
#include "core/State.h"
#include "core/StateManager.h"

class MainMenuState : public State
{
private:
    GameData &gameData;
    StateManager &stateManager;
    sf::RenderWindow &window;
    sf::View menuView;

    sf::Text gameTitle;

    float buttonSpacing;
    std::vector<Button> buttons;
    int selectedButtonIndex = 0;

private:
    void addButton(std::string id,
                   std::string label,
                   std::function<void()> callback);
    void updateMenuItemPositions();

public:
    MainMenuState(GameData &data, StateManager &manager, sf::RenderWindow &window);
    ~MainMenuState();

    void handleEvent(const sf::Event &event) override;
    void handleWindowResize(sf::Vector2u newSize) override;
    void update(sf::Time deltaTime) override;
    void render() override;
};

#endif // MAINMENUSTATE_H
