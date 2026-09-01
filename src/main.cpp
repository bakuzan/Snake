#include <SFML/Graphics.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <signal.h>

#include "constants/Constants.h"
#include "core/GameData.h"
#include "core/StateManager.h"
#include "states/MainMenuState.h"

void LoadWindowIcon(sf::Window &window)
{
    sf::Image icon;
    if (!icon.loadFromFile("resources/images/icon.png"))
    {
        exit(1);
    }

    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}

void signalHandler(int sig)
{
    std::cerr << "Caught signal: " << sig << std::endl;
    exit(sig);
}

int main()
{
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);

    std::srand(static_cast<int>(std::time(nullptr))); // Seed for rand

    sf::VideoMode viewSize(static_cast<unsigned int>(Constants::VIEW_WIDTH),
                           static_cast<unsigned int>(Constants::VIEW_HEIGHT));
    sf::RenderWindow window(viewSize, Constants::PROJECT_NAME);
    LoadWindowIcon(window);

    GameData gameData;
    StateManager stateManager;

    // Push initial state (GameState)
    stateManager.pushState(std::make_unique<MainMenuState>(gameData, stateManager, window));

    bool isFullscreen = false;
    sf::Vector2u windowedSize = window.getSize();

    auto recreateWindow = [&](bool fullscreen)
    {
        if (fullscreen)
        {
            auto mode = sf::VideoMode::getDesktopMode();
            window.create(mode, Constants::PROJECT_NAME, sf::Style::Fullscreen);
        }
        else
        {
            auto windowedMode = sf::VideoMode(windowedSize.x, windowedSize.y);
            window.create(windowedMode, Constants::PROJECT_NAME, sf::Style::Default);
        }

        LoadWindowIcon(window);
        stateManager.handleWindowResize(window.getSize());
    };

    sf::Clock clock;
    float accumulator = 0.0f;
    const float fixedTimeStep = 1.0f / 60.0f;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                stateManager.handleWindowResize(window.getSize());
            }
            else if (event.type == sf::Event::KeyPressed &&
                     event.key.code == sf::Keyboard::F11)
            {
                if (!isFullscreen)
                {
                    // remember last windowed size
                    windowedSize = window.getSize();
                }

                isFullscreen = !isFullscreen;
                recreateWindow(isFullscreen);
                continue; // window recreated; skip passing this event to states
            }

            // Input
            stateManager.handleEvent(event);
        }

        // Update (Logic)
        sf::Time deltaTime = clock.restart();
        accumulator += deltaTime.asSeconds();

        while (accumulator >= fixedTimeStep)
        {
            stateManager.update(sf::seconds(fixedTimeStep));
            accumulator -= fixedTimeStep;
        }

        // Draw and Display
        window.clear();
        stateManager.render();
        window.display();
    }

    return 0;
}
