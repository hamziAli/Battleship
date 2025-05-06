// Here we will add all the function prototypes of components of the game such as grid,
// player vs player , player vs computer and ships
// here we will do decelaration of the function and classes created in the header file
#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H
#include <vector>
#include <string>
using namespace std;
const int Board_Size = 10;
const int Ships = 5; // Carrier(5x1),Battleship(4x1),Destroyer(3x1),Submarine(3x1)Patrol(2x1),Boat(1x1)

enum cell_status
{
    EMPTY,
    SHIP,
    HIT,
    MISS
};

struct Coordinate
{
    int rows;
    int cols;
};

class Ship
{
public:
    string name;
    int size;
    Coordinate start; // Let’s say we have a 3-cell long ship placed horizontally at (row = 2, col = 4).
    bool isHorizontal;
    int hitsTaken;

    Ship(string name, int size);
    bool isSunk() const; // This is a member function of the Ship class that tells you whether the ship has been completely destroyed (sunk)
};

class Board
{
public:
    cell_status **grid;
    vector<Ship> Ships;
    void display(bool showShips = true) const;     // Checks if the ship can be placed at its starting point and direction
                                                   // Returns false if it's out of bounds or overlaps another ship
    bool placeShip(const Ship &ship);              // Places the ship on the board if the position is valid
                                                   // Updates the grid and adds the ship to the ship list
    bool isValidPlacement(const Ship &ship) const; // Simulates an attack on a specific cell
                                                   // Marks HIT if ship is present, MISS if empty
                                                   // Returns true if it's a hit, false otherwise
    bool attackCell(int row, int col);

public:
    Board();
    ~Board();
};

#endif
// Purpose:
// Declares the core components (classes and types) used in the Battleship game, including the board, ships, and helper structures.
