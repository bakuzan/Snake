#ifndef GAMEMENUSTATE_H
#define GAMEMENUSTATE_H

#include <SFML/Graphics.hpp>
#include <string>

#include "ui/Button.h"
#include "core/GameData.h"
#include "core/State.h"
#include "core/StateManager.h"

class GameMenuState : public State
{
private:
    GameData &gameData;
    StateManager &stateManager;
    sf::RenderWindow &window;
    sf::View pauseView;

    sf::RectangleShape background;
    sf::Text pauseText;

    float buttonSpacing;
    std::vector<Button> buttons;
    int selectedButtonIndex = 0;

private:
    void addButton(std::string id,
                   std::string label,
                   std::function<void()> callback);
    void updateMenuItemPositions();

public:
    GameMenuState(GameData &data, StateManager &manager, sf::RenderWindow &window);
    ~GameMenuState();

    void handleEvent(const sf::Event &event) override;
    void handleWindowResize(sf::Vector2u newSize) override;
    void update(sf::Time deltaTime) override;
    void render() override;
};

#endif // GAMEMENUSTATE_H
