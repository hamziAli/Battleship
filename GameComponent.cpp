// here we will do decelaration of the function and classes created in the header file
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "GameComponent.h"
using namespace std;
// ===== Ship class =====
Ship::Ship(string name, int size)
{
    this->name = name;
    this->size = size;
    hitsTaken = 0;
    isHorizontal = true; // default
    start = {0, 0};      // default
}

bool Ship::isSunk() const
{
    return hitsTaken >= size;
}

// ===== Board class =====

Board::Board()
{
    // Allocate memory for grid
    grid = new cell_status *[Board_Size];
    for (int i = 0; i < Board_Size; ++i)
    {
        grid[i] = new cell_status[Board_Size];
    }

    // Initialize all cells to EMPTY
    for (int row = 0; row < Board_Size; ++row)
    {
        for (int col = 0; col < Board_Size; ++col)
        {
            grid[row][col] = EMPTY;
        }
    }
}

Board::~Board()
{
    for (int i = 0; i < Board_Size; ++i)
    {
        delete[] grid[i];
    }
    delete[] grid;
}
int main()
{

    return 0;
}