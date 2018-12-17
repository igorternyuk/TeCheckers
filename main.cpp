#include "game.hpp"

int main(int argc, char *argv[])
{
    Game::getInstance().run(argc, argv);
    return 0;
}
