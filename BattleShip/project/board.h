#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <chrono>
#include <thread>
#include <limits>
#include <vector>

class Player; // Forward declaration of Player

// Constants
const int BOARD_SIZE = 8;
const char EMPTY_CHAR = 249;
const char MISS_CHAR = 176;  // For missed shots
const char HIT_CHAR = 254;   // For hit ships
const char WATER_CHAR = 249; // Empty water

// Direction enum for ships
enum Direction
{
    LEFT = 0,
    RIGHT = 1,
    UP = 2,
    DOWN = 3
};

// Position class to represent coordinates
class Position
{
public:
    int x;
    int y;

    Position();
    Position(int xPos, int yPos);

    bool isValid() const;
    bool operator==(const Position &other) const;
};

// Ship class
class Ship
{
private:
    char type;
    int length;
    int hitsRemaining;
    std::vector<Position> positions;

public:
    Ship(char shipType, int shipLength);

    char getType() const;
    int getLength() const;
    int getHitsRemaining() const;

    bool isDestroyed() const;
    void hit();
    void addPosition(const Position &pos);
    const std::vector<Position> &getPositions() const;
};

// Board class to represent the game grid
class Board
{
private:
    char grid[BOARD_SIZE][BOARD_SIZE];
    std::vector<Ship> ships;

public:
    Board();

    void clearBoard();
    char getCell(int x, int y) const;
    void setCell(int x, int y, char value);
    bool isEmptyCell(int x, int y) const;
    bool isValidPlacement(int x, int y, int length, Direction dir) const;
    void placeShip(Ship &ship, int x, int y, Direction dir);
    void placeRandomShips(bool isEnemyBoard);
    bool processShot(int x, int y);
    bool allShipsDestroyed() const;
    bool isShipDestroyed(char shipType) const;
    const std::vector<Ship> &getShips() const;
};

// UI class to handle display
class UI
{
public:
    static void clearScreen();
    static void delay(int milliseconds);
    static void Battleshiplogo();
    static void Battleshiplogo2();
    static void drawGameBoard(const Player &player, const Player &opponent);
    static void displayGameRules();
    static void displayMainMenu(bool cpuSmartMode);
    static void displayShipPlacementMenu();
    static void displayGameOver(const Player &player, const Player &cpu);
    static Position getPlayerTarget();
    static void displayTurnIndicator(bool playerTurn);
    static void displayShipDestroyed(char shipType, const std::string &fullShipName, const std::string &contextPrefix);
    static void clearInputBuffer();
    static void displayPlacingShip(const std::string &shipName, int length);
    static Position getShipStartPosition();
    static Direction getShipDirection();
    static void displayPlayerBoard(const Board &board);
    static void loadingEffect(const std::string &message, int dotCount = 3, int delayMs = 500);
    static void spinner(int durationMs);
};

#endif