#ifndef GAME_H
#define GAME_H
#include "player.h"
#include "board.h"
#include <vector> 
#include <string> 


class Game
{
private:
    Player player;
    Player cpu;
    bool gameOver;
    const int winningScore = 17;

    // smarter cpu state
    Position lastHit = Position(-1, -1);

    std::vector<Position> huntTargets;
    bool hunting = false;
   
    bool cpuSmartMode = false; 

    // To track announced sunk ships per game
    std::vector<char> playerShipsSunkThisGame;
    std::vector<char> cpuShipsSunkThisGame;

    void cpuSmartTurn();
    void quickplayDemo();
    std::string getFullShipName(char shipType, bool isEnemy); // Helper to get full ship name


// Constructor and main game execution method    
public:
    Game();
    void run();

// Game setup and turn-handling functions (initialization, ship placement, player & CPU turns, gameplay loop)
private:
    void initialize();
    void placePlayerShips();
    void manualShipPlacement(); 
    void playerTurn();
    void cpuTurn();
    void play();
    std::vector<Position> getAdjacentPositions(const Board &board, int x, int y); // Retained for smart CPU
};

#endif