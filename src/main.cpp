#include "core/Game.h"
#include "states/MenuState.h"
#include <iostream>

int main() {
    try {
        std::cout << "=== Zombie Don't Kill Me ===" << std::endl;
        std::cout << "Starting up..." << std::endl;

        Game game;

        // Start with the menu state
        game.getStateManager().pushState(std::make_unique<MenuState>(game));

        game.run();

        std::cout << "Game exited cleanly." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
