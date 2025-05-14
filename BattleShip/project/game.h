#ifndef GAME_H
#define GAME_H
#include "player.h"
#include "board.h"
#include <vector> // For std::vector
#include <string> // For std::string

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
    // int huntDirection = -1; // This was from an older CPU logic, can be removed if not used
    std::vector<Position> huntTargets;
    bool hunting = false;
    // std::vector<Position> cpuTried; // This was from an older CPU logic, can be removed if not used

    bool cpuSmartMode = false; // Toggle for smart CPU

    // To track announced sunk ships per game
    std::vector<char> playerShipsSunkThisGame;
    std::vector<char> cpuShipsSunkThisGame;

    void cpuSmartTurn();
    void quickplayDemo();
    std::string getFullShipName(char shipType, bool isEnemy); // Helper to get full ship name

public:
    Game();
    void run();

private:
    void initialize();
    void placePlayerShips();
    void manualShipPlacement(); // Keep as void since we handle the back option internally
    void playerTurn();
    void cpuTurn();
    void play();
    std::vector<Position> getAdjacentPositions(const Board &board, int x, int y); // Retained for smart CPU
};

#endif