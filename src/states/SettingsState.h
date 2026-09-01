#ifndef SETTINGSSTATE_H
#define SETTINGSSTATE_H

#include <SFML/Graphics.hpp>

#include <vector>

#include "ui/Button.h"
#include "ui/ToggleOption.h"
#include "core/GameData.h"
#include "core/State.h"
#include "core/StateManager.h"
#include "data/UIOption.h"

class SettingsState : public State
{
private:
    GameData &gameData;
    StateManager &stateManager;
    sf::RenderWindow &window;
    sf::View settingsView;

    sf::Text gameTitle;

    std::vector<ToggleOption> toggleOptions;

    float buttonSpacing;
    std::vector<Button> buttons;
    int selectedButtonIndex;

    bool shouldReturnToMenuState;

private:
    void updateMenuItemPositions();

public:
    SettingsState(GameData &data, StateManager &manager, sf::RenderWindow &win);
    ~SettingsState();

    void handleEvent(const sf::Event &event) override;
    void handleWindowResize(sf::Vector2u newSize) override;
    void update(sf::Time deltaTime) override;
    void render() override;
};

#endif // SETTINGSSTATE_H
