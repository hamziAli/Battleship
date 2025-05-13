#ifndef GAME_H
#define GAME_H
#include "player.h"
#include "board.h"

// Game class to coordinate gameplay
class Game
{
private:
    Player player;
    Player cpu;
    bool gameOver;
    const int winningScore = 17; // Total number of ship cells

    // Smarter CPU state
    Position lastHit = Position(-1, -1);
    int huntDirection = -1;            // 0=left, 1=right, 2=up, 3=down
    std::vector<Position> huntTargets; // candidate targets after a hit
    bool hunting = false;
    std::vector<Position> cpuTried; // for smarter randomization

    bool cpuSmartMode = false; // Toggle for smart CPU

    void cpuSmartTurn();
    void quickplayDemo();

public:
    Game();
    void run();

private:
    void initialize();
    void placePlayerShips();
    void manualShipPlacement();
    void playerTurn();
    void cpuTurn();
    void play();
};

#endif