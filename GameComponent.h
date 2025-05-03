// Here we will add all the function prototypes of components of the game such as grid,
// player vs player , player vs computer and ships
// here we will do decelaration of the function and classes created in the header file
#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H
#include <vector>
#include <string>
using namespace std;
const int Board_Size = 10;
const int Ships = 9; // 5(king) , 4(Queen) , 3(Bishop) , 2(2xKnights) , 1(4xPawns)
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
private:
    cell_status **grid;
    vector<Ship> Ship;

public:
    Board();
    ~Board();
};

#endif