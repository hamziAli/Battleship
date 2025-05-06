#include "GameComponent.h"
#include <iostream>
using namespace std;

// Ship class constructor
Ship::Ship(string name, int size) : name(name), size(size), hitsTaken(0), isHorizontal(true)
{
    start = {0, 0}; // Default starting position
}

// Check if the ship is sunk
bool Ship::isSunk() const
{
    return hitsTaken >= size;
}

// Board class constructor
Board::Board()
{
    // Allocate memory for the grid
    grid = new cell_status *[Board_Size];
    for (int i = 0; i < Board_Size; ++i)
    {
        grid[i] = new cell_status[Board_Size];
        for (int j = 0; j < Board_Size; ++j)
        {
            grid[i][j] = EMPTY; // Initialize all cells to EMPTY
        }
    }
}

// Board class destructor
Board::~Board()
{
    // Deallocate memory for the grid
    for (int i = 0; i < Board_Size; ++i)
    {
        delete[] grid[i];
    }
    delete[] grid;
}

// Display the board
void Board::display(bool showShips) const
{
    cout << "  ";
    for (int col = 0; col < Board_Size; ++col)
    {
        cout << col << " ";
    }
    cout << endl;

    for (int row = 0; row < Board_Size; ++row)
    {
        cout << row << " ";
        for (int col = 0; col < Board_Size; ++col)
        {
            if (grid[row][col] == SHIP && showShips)
            {
                cout << "0 ";
            }
            else if (grid[row][col] == HIT)
            {
                cout << "X ";
            }
            else if (grid[row][col] == MISS)
            {
                cout << "* ";
            }
            else
            {
                cout << ". ";
            }
        }
        cout << endl;
    }
}

// Check if a ship can be placed
bool Board::isValidPlacement(const Ship &ship) const
{
    int row = ship.start.rows;
    int col = ship.start.cols;

    for (int i = 0; i < ship.size; ++i)
    {
        if (row >= Board_Size || col >= Board_Size || grid[row][col] != EMPTY)
        {
            return false;
        }
        if (ship.isHorizontal)
        {
            ++col;
        }
        else
        {
            ++row;
        }
    }
    return true;
}

// Place a ship on the board
bool Board::placeShip(const Ship &ship)
{
    if (!isValidPlacement(ship))
    {
        return false;
    }

    int row = ship.start.rows;
    int col = ship.start.cols;

    for (int i = 0; i < ship.size; ++i)
    {
        grid[row][col] = SHIP;
        if (ship.isHorizontal)
        {
            ++col;
        }
        else
        {
            ++row;
        }
    }

    Ships.push_back(ship);
    return true;
}

// Simulate an attack on the board
bool Board::attackCell(int row, int col)
{
    if (row < 0 || row >= Board_Size || col < 0 || col >= Board_Size)
    {
        return false;
    }

    if (grid[row][col] == SHIP)
    {
        grid[row][col] = HIT;
        return true;
    }
    else if (grid[row][col] == EMPTY)
    {
        grid[row][col] = MISS;
        return false;
    }

    return false; // Cell already attacked
}

int main()
{
    // Create a board
    Board board;

    // Create a ship
    Ship ship("Destroyer", 3);
    ship.start = {2, 4};      // Set starting position
    ship.isHorizontal = true; // Set orientation

    // Place the ship on the board
    if (board.placeShip(ship))
    {
        cout << "Ship placed successfully!" << endl;
    }
    else
    {
        cout << "Failed to place the ship." << endl;
    }

    // Display the board
    board.display();

    // Simulate an attack
    if (board.attackCell(2, 4))
    {
        cout << "Hit!" << endl;
    }
    else
    {
        cout << "Miss!" << endl;
    }

    // Display the board after the attack
    board.display();

    return 0;
}