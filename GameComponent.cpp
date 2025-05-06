#include <iostream>
#include <vector>
#include <string>
#include "GameComponent.h"
using namespace std;

// Ship class represents a single ship in the Battleship game
class Ship {
public:
    string name;    // Stores the name of the ship (e.g., "Destroyer")
    int size;       // Number of grid cells the ship occupies
    int hits;       // Tracks how many times the ship has been hit
    bool horizontal;// True if ship is placed horizontally, false if vertical
    Coordinate start; // Starting position (row, column) of the ship on the grid

    // Constructor to initialize a ship with a name and size
    Ship(string shipName, int shipSize) {
        name = shipName;        // Set the ship's name
        size = shipSize;        // Set the ship's size
        hits = 0;               // Initialize hits to 0 (no hits yet)
        horizontal = true;      // Default to horizontal placement
        start = {0, 0};         // Default starting position at top-left (0,0)
    }

    // Checks if the ship is sunk (i.e., hits equal or exceed size)
    bool isSunk() const {
        return hits >= size;    // Ship is sunk if it has been hit enough times
    }
};

// Board class represents the game board where ships are placed and attacked
class Board {
private:
    static const int SIZE = 10;      // Defines the board as a 10x10 grid
    cell_status grid[SIZE][SIZE];    // 2D array to store the state of each cell
    vector<Ship> ships;              // List of ships placed on the board

public:
    // Constructor to initialize the game board
    Board() {
        // Loop through each cell in the grid
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                grid[row][col] = EMPTY; // Set each cell to EMPTY initially
            }
        }
    }

    // Displays the board to the console
    // showShips: if true, displays ships; if false, hides them (for opponent's view)
    void display(bool showShips) const {
        // Print column numbers (0 to 9) for reference
        cout << "  ";
        for (int col = 0; col < SIZE; col++) {
            cout << col << " ";
        }
        cout << endl;

        // Print each row of the board
        for (int row = 0; row < SIZE; row++) {
            cout << row << " "; // Print row number
            for (int col = 0; col < SIZE; col++) {
                // Decide what to display based on cell status
                if (grid[row][col] == SHIP && showShips)
                    cout << "S "; // Show ship if allowed
                else if (grid[row][col] == HIT)
                    cout << "X "; // Show hit (successful attack)
                else if (grid[row][col] == MISS)
                    cout << "O "; // Show miss (unsuccessful attack)
                else
                    cout << ". "; // Show empty cell
            }
            cout << endl; // Move to next line after each row
        }
    }

    // Checks if a ship can be placed at its specified position
    bool canPlaceShip(const Ship &ship) const {
        int row = ship.start.rows; // Get starting row
        int col = ship.start.cols; // Get starting column

        // Check each cell the ship will occupy
        for (int i = 0; i < ship.size; i++) {
            // Check if the position is outside the board
            if (row >= SIZE || col >= SIZE) {
                return false; // Invalid: out of bounds
            }
            // Check if the cell is already occupied (not empty)
            if (grid[row][col] != EMPTY) {
                return false; // Invalid: cell is occupied
            }
            // Move to the next cell based on ship orientation
            if (ship.horizontal)
                col++; // Move right for horizontal
            else
                row++; // Move down for vertical
        }
        return true; // Placement is valid
    }

    // Places a ship on the board if possible
    bool placeShip(Ship ship) {
        // First, verify if the ship can be placed
        if (!canPlaceShip(ship)) {
            return false; // Cannot place ship
        }

        int row = ship.start.rows; // Get starting row
        int col = ship.start.cols; // Get starting column

        // Place the ship by marking its cells as SHIP
        for (int i = 0; i < ship.size; i++) {
            grid[row][col] = SHIP; // Mark cell as containing a ship
            // Move to the next cell based on orientation
            if (ship.horizontal)
                col++; // Move right
            else
                row++; // Move down
        }

        // Add the ship to the list of ships
        ships.push_back(ship);
        return true; // Ship placed successfully
    }

    // Attacks a specific cell on the board
    bool attack(int row, int col) {
        // Check if the attack position is valid
        if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
            return false; // Invalid: out of bounds
        }

        // If the cell contains a ship, mark it as hit
        if (grid[row][col] == SHIP) {
            grid[row][col] = HIT; // Mark as hit
            return true;          // Attack was a hit
        }
        // If the cell is empty, mark it as a miss
        else if (grid[row][col] == EMPTY) {
            grid[row][col] = MISS; // Mark as miss
            return false;          // Attack was a miss
        }
        // If already hit or missed, do nothing
        return false; // No change (already attacked)
    }
};

// Main function to demonstrate the Battleship game
int main() {
    // Create a new game board
    Board board;

    // Create a sample ship called "Destroyer" with size 3
    Ship destroyer("Destroyer", 3);
    destroyer.start = {2, 2}; // Place it at row 2, column 2
    destroyer.horizontal = true; // Place horizontally

    // Attempt to place the ship on the board
    if (board.placeShip(destroyer)) {
        cout << "Ship placed successfully!" << endl;
    } else {
        cout << "Failed to place ship." << endl;
    }

    // Display the board, showing the ship
    cout << "Board with ships:" << endl;
    board.display(true);

    // Simulate an attack at position (2,2)
    cout << "\nAttacking position (2,2):" << endl;
    if

 (board.attack(2, 2)) {
        cout << "Hit!" << endl; // Print if attack was successful
    } else {
        cout << "Miss!" << endl; // Print if attack missed
    }

    // Display the board after the attack
    cout << "\nBoard after attack:" << endl;
    board.display(true);

    return 0; // End the program
}