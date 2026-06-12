#include "Game.hpp"

// Точка входа: инициализация и запуск игрового цикла.
int main() {
    Game game;
    if (!game.init()) return 1;
    game.run();
    return 0;
}
