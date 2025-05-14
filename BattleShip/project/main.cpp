#include "game.h"
#include "player.h" // Assumed header for Player class
#include "board.h"  // Assumed header for Board class

#include <iostream>
#include <cstdlib>
#include <ctime>
int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    Game battleship;
    battleship.run();

    return 0;
}