#include "game.h"
#include "player.h"
#include "board.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
int main()
{
    // random number generator with  time
    srand(static_cast<unsigned int>(time(nullptr)));

    Game battleship;  //  game instance
    battleship.run(); // start the game

    return 0;
}