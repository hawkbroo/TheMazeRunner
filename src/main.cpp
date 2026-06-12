// The Maze Runner — курсовой проект на C++ и SFML
#include "Game.hpp"

int main() {
    Game game;
    if (!game.init()) return 1;
    game.run();
    return 0;
}
