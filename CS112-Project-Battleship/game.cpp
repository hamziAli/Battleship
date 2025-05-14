#include "game.h"
#include "player.h"
#include "board.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Game class implementation for Battleship game logic
// Handles game setup, player and CPU turns, AI logic, and main game loop
Game::Game() : player("Player"), cpu("CPU"), gameOver(false), cpuSmartMode(true), hunting(false) {}

// Returns the full ship name based on type and owner (player or enemy)
std::string Game::getFullShipName(char shipType, bool isEnemy)
{
    if (isEnemy)
    {
        switch (shipType)
        {
        case 'A':
            return "Alpha";
        case 'B':
            return "Bravo";
        case 'C':
            return "Charlie";
        case 'D':
            return "Delta";
        case 'E':
            return "Echo";
        default:
            return "Unknown Enemy Ship";
        }
    }
    else
    {
        switch (shipType)
        {
        case 'T':
            return "PNS Tughril";
        case 'Z':
            return "PNS Zulfiqar";
        case 'H':
            return "PNS Hangor";
        case 'Y':
            return "PNS Yarmuk";
        case 'M':
            return "PNS Mujahid";
        default:
            return "Unknown Player Ship";
        }
    }
}

// Sets up a new game: clears boards, resets scores, places ships
void Game::initialize()
{
    player.getOwnBoard().clearBoard();
    player.getTrackingBoard().clearBoard();
    cpu.getOwnBoard().clearBoard();
    cpu.getTrackingBoard().clearBoard();

    player.resetScore();
    cpu.resetScore();

    playerShipsSunkThisGame.clear();
    cpuShipsSunkThisGame.clear();

    cpu.getOwnBoard().placeRandomShips(true);
    placePlayerShips();
    if (gameOver)
        return;

    gameOver = false;
}

// Presents ship placement menu and handles user choice
void Game::placePlayerShips()
{
    int choice;
    while (true)
    {
        UI::clearScreen();
        UI::displayShipPlacementMenu();
        if (!(std::cin >> choice))
        {
            std::cout << "\tInvalid input. Please enter a number.\n";
            UI::clearInputBuffer();
            UI::delay(1000);
            continue;
        }
        switch (choice)
        {
        case 1:
            player.getOwnBoard().placeRandomShips(false);
            UI::clearScreen();
            UI::displayPlayerBoard(player.getOwnBoard());
            std::cout << "\n\tShips placed randomly. Press Enter to continue...";
            UI::clearInputBuffer();
            std::cin.get();
            return;
        case 2:
            manualShipPlacement();
            if (gameOver)
                return;
            return;
        case 3:
            gameOver = true;
            return;
        default:
            std::cout << "\tInvalid choice. Please enter a number between 1 and 3.\n";
            UI::delay(1000);
            break;
        }
    }
}

// Allows manual placement of each ship with input validation
void Game::manualShipPlacement()
{
    // ship types and lengths
    struct ShipDef
    {
        char type;
        int length;
        std::string name;
    };

    ShipDef shipDefs[] = {
        {'T', 5, "PNS Tughril"},
        {'Z', 4, "PNS Zulfiqar"},
        {'H', 3, "PNS Hangor"},
        {'Y', 3, "PNS Yarmuk"},
        {'M', 2, "PNS Mujahid"}};

    // Loop through each ship defined for the player
    for (const auto &def : shipDefs)
    {
        bool placed = false;
        // Loop until the current ship is successfully placed
        while (!placed)
        {
            UI::displayPlayerBoard(player.getOwnBoard());
            UI::displayPlacingShip(def.name, def.length);
            Position pos = UI::getShipStartPosition();
            // Allow user to go back to main menu by entering specific coordinates
            if (pos.x == -2 && pos.y == -2)
            {
                gameOver = true;
                return;
            }
            // Basic validation for position coordinates
            if (!pos.isValid())
            {
                std::cout << "\tInvalid position. Try again.\n";
                UI::delay(1000);
                continue;
            }
            Direction dir = UI::getShipDirection();
            // Allow user to go back to main menu from direction input
            if (static_cast<int>(dir) == -2)
            {
                gameOver = true;
                return;
            }
            // Validate the chosen direction
            if (static_cast<int>(dir) < 0 || static_cast<int>(dir) > 3)
            {
                std::cout << "\tInvalid direction selection. Try again.\n";
                UI::delay(1000);
                continue;
            }
            // Check if the ship can be placed at the chosen position and direction without overlap or going out of bounds
            if (player.getOwnBoard().isValidPlacement(pos.x, pos.y, def.length, dir))
            {
                Ship ship(def.type, def.length);
                player.getOwnBoard().placeShip(ship, pos.x, pos.y, dir);
                placed = true;
                std::cout << "\t" << def.name << " placed successfully!\n";
                UI::delay(1000);
            }
            else
            {
                std::cout << "\tInvalid placement. Ship would overlap or go out of bounds. Try again.\n";
                UI::delay(1500);
            }
        }
    }
    UI::displayPlayerBoard(player.getOwnBoard());
    std::cout << "\n\tAll ships placed! Press Enter to continue...";
    UI::clearInputBuffer();
    std::cin.get();
}

// Returns valid adjacent positions for smart CPU targeting
std::vector<Position> Game::getAdjacentPositions(const Board &board, int x, int y)
{
    std::vector<Position> adj;
    if (x > 0)
        adj.push_back(Position(x - 1, y));
    if (x < BOARD_SIZE - 1)
        adj.push_back(Position(x + 1, y));
    if (y > 0)
        adj.push_back(Position(x, y - 1));
    if (y < BOARD_SIZE - 1)
        adj.push_back(Position(x, y + 1));
    return adj;
}

// Handles the player's attack turn, including input validation and feedback
void Game::playerTurn()
{
    UI::displayTurnIndicator(true);
    // Loop to allow player to take turns until a miss or game over
    while (true)
    {
        Position target = UI::getPlayerTarget();
        // Allow user to go back to main menu by entering specific coordinates
        if (target.x == -2 && target.y == -2)
        {
            gameOver = true;
            return;
        }
        if (!target.isValid())
        {
            std::cout << "\tInvalid coordinates. Try again.\n";
            UI::delay(1000);
            continue;
        }
        // Check if the cell has already been fired upon (is not empty or marked as water from a previous miss)
        if (player.getTrackingBoard().getCell(target.x, target.y) != EMPTY_CHAR &&
            player.getTrackingBoard().getCell(target.x, target.y) != WATER_CHAR) // Check against EMPTY_CHAR or specific water char
        {
            std::cout << "\tYou already fired at this position. Try again.\n";
            UI::delay(1000);
            continue;
        }

        bool hit = player.attack(cpu, target.x, target.y);
        UI::drawGameBoard(player, cpu);
        std::cout << "\n\tTarget " << static_cast<char>('A' + target.y) << (target.x + 1) << ": ";

        if (hit)
        {
            std::cout << "HIT!\n";
            UI::delay(1000);
            // Check if any CPU ships were sunk by this hit
            for (const auto &ship : cpu.getOwnBoard().getShips())
            {
                // Announce sunk ship only once per game
                if (ship.isDestroyed() &&
                    std::find(cpuShipsSunkThisGame.begin(), cpuShipsSunkThisGame.end(), ship.getType()) == cpuShipsSunkThisGame.end())
                {
                    cpuShipsSunkThisGame.push_back(ship.getType());
                    std::string shipName = getFullShipName(ship.getType(), true); // true for enemy
                    UI::displayShipDestroyed(ship.getType(), shipName, "\n\t!!! ENEMY SHIP DESTROYED !!!\n\t");
                }
            }
            // Check for win condition
            if (player.getScore() >= winningScore)
            {
                gameOver = true;
                return;
            }
            // Player gets another turn if it was a hit
            std::cout << "\n\tYou get another turn!\n";
            UI::delay(1000);
        }
        else
        {
            std::cout << "MISS!\n";
            UI::delay(1000);
            break; // End player's turn on a miss
        }
    }
}

// Handles CPU's turn with random targeting (if not in smart mode)
void Game::cpuTurn()
{
    if (cpuSmartMode)
    {
        cpuSmartTurn(); // Use smart AI if enabled
        return;
    }

    UI::displayTurnIndicator(false);
    // Loop to allow CPU to take turns until a miss or game over
    while (true)
    {
        int x, y;
        // Randomly select coordinates until an untargeted cell is found
        do
        {
            x = rand() % BOARD_SIZE;
            y = rand() % BOARD_SIZE;
        } while (cpu.getTrackingBoard().getCell(x, y) != EMPTY_CHAR &&
                 cpu.getTrackingBoard().getCell(x, y) != WATER_CHAR); // Ensure cell hasn't been hit or missed before

        std::string target_coord_str = std::string(1, static_cast<char>('A' + y)) + std::to_string(x + 1);
        UI::loadingEffect("\n\tCPU targeting " + target_coord_str, 4, 500);

        bool hit = cpu.attack(player, x, y);
        UI::drawGameBoard(player, cpu);
        std::cout << "\n\tTarget " << target_coord_str << ": ";
        UI::delay(700);
        if (hit)
        {
            std::cout << "HIT!\n";
            UI::delay(1000);
            // Check if any player ships were sunk by this hit
            for (const auto &ship : player.getOwnBoard().getShips())
            {
                // Announce sunk ship only once per game
                if (ship.isDestroyed() &&
                    std::find(playerShipsSunkThisGame.begin(), playerShipsSunkThisGame.end(), ship.getType()) == playerShipsSunkThisGame.end())
                {
                    playerShipsSunkThisGame.push_back(ship.getType());
                    std::string shipName = getFullShipName(ship.getType(), false);
                    UI::displayShipDestroyed(ship.getType(), shipName, "\n\t!!! YOUR SHIP DESTROYED !!!\n\t");
                }
            }
            // Check for win condition
            if (cpu.getScore() >= winningScore)
            {
                gameOver = true;
                return;
            }
            // CPU gets another turn if it was a hit
            std::cout << "\n\tCPU gets another turn!\n";
            UI::delay(1000);
        }
        else
        {
            std::cout << "MISS!\n";
            UI::delay(1000);
            break; // End CPU's turn on a miss
        }
    }
}

// Smart CPU turn: hunts for ships after a hit, otherwise fires randomly
void Game::cpuSmartTurn()
{
    UI::displayTurnIndicator(false);
    // Loop to allow CPU to take turns until a miss or game over
    while (true)
    {
        int x = -1, y = -1;
        // If in 'hunting' mode (a previous hit was made and adjacent targets are available)
        if (hunting && !huntTargets.empty())
        {
            Position p = huntTargets.back(); // Get the most recently added hunt target
            huntTargets.pop_back();
            x = p.x;
            y = p.y;
            // If this hunt target has already been fired upon (e.g., by another path in hunting), skip and get next target
            if (cpu.getTrackingBoard().getCell(x, y) != EMPTY_CHAR &&
                cpu.getTrackingBoard().getCell(x, y) != WATER_CHAR)
            {
                continue;
            }
        }
        else
        {
            // Not hunting or no hunt targets left, switch to random targeting
            hunting = false;
            // Randomly select coordinates until an untargeted cell is found
            do
            {
                x = rand() % BOARD_SIZE;
                y = rand() % BOARD_SIZE;
            } while (cpu.getTrackingBoard().getCell(x, y) != EMPTY_CHAR &&
                     cpu.getTrackingBoard().getCell(x, y) != WATER_CHAR); // Ensure cell hasn't been hit or missed before
        }

        std::string target_coord_str = std::string(1, static_cast<char>('A' + y)) + std::to_string(x + 1);
        UI::loadingEffect("\n\tCPU targeting " + target_coord_str, 4, 500);

        bool hit = cpu.attack(player, x, y);
        UI::drawGameBoard(player, cpu);
        std::cout << "\n\tTarget " << target_coord_str << ": ";
        UI::delay(700);
        if (hit)
        {
            std::cout << "HIT!\n";
            UI::delay(1000);
            lastHit = Position(x, y); // Record the position of the successful hit
            hunting = true;           // Switch to hunting mode
            // Get adjacent positions to the hit cell as potential next targets
            std::vector<Position> potentialTargets = getAdjacentPositions(player.getOwnBoard(), x, y);
            for (const auto &pt : potentialTargets)
            {
                // Add to huntTargets only if it's a valid, untargeted cell and not already in huntTargets
                if (cpu.getTrackingBoard().getCell(pt.x, pt.y) == EMPTY_CHAR ||
                    cpu.getTrackingBoard().getCell(pt.x, pt.y) == WATER_CHAR)
                {
                    bool alreadyInTargets = false;
                    for (const auto &ht : huntTargets)
                        if (ht.x == pt.x && ht.y == pt.y)
                        {
                            alreadyInTargets = true;
                            break;
                        }
                    if (!alreadyInTargets)
                        huntTargets.push_back(pt);
                }
            }

            // Check if any player ships were sunk by this hit
            for (const auto &ship : player.getOwnBoard().getShips())
            {
                // Announce sunk ship only once per game
                if (ship.isDestroyed() &&
                    std::find(playerShipsSunkThisGame.begin(), playerShipsSunkThisGame.end(), ship.getType()) == playerShipsSunkThisGame.end())
                {
                    playerShipsSunkThisGame.push_back(ship.getType());
                    std::string shipName = getFullShipName(ship.getType(), false); // false for player
                    UI::displayShipDestroyed(ship.getType(), shipName, "\n\t!!! YOUR SHIP DESTROYED !!!\n\t");
                }
            }
            // Check for win condition
            if (cpu.getScore() >= winningScore)
            {
                gameOver = true;
                return;
            }
            // CPU gets another turn if it was a hit
            std::cout << "\n\tCPU gets another turn!\n";
            UI::delay(1000);
        }
        else
        {
            std::cout << "MISS!\n";
            UI::delay(1000);
            // On a miss, if there are no more hunt targets, hunting mode naturally ends on the next iteration.
            // If there were hunt targets, the CPU will continue trying them.
            break; // End CPU's turn on a miss
        }
    }
}

// Main game loop: alternates player and CPU turns until game over
void Game::play()
{
    initialize();
    if (gameOver) // This handles cases like backing out of ship placement
    {
        return;
    }

    while (!gameOver) // Loop continues as long as the game is not over
    {
        UI::drawGameBoard(player, cpu);

        std::cout << "\n\tScore - Player: " << player.getScore() << "/17  CPU: " << cpu.getScore() << "/17\n";

        playerTurn(); // playerTurn can set gameOver to true if player wins or quits

        if (gameOver) // If player's turn ended the game (win/quit)
        {
            break; // Exit the loop to show game over screen
        }

        // If game is not over, CPU takes its turn
        cpuTurn(); // cpuTurn can set gameOver to true if CPU wins

        if (gameOver) // If CPU's turn ended the game (win)
        {
            break; // Exit the loop to show game over screen
        }
        // If neither turn ended the game, the loop continues
    }

    // This block is now correctly reached when gameOver is true
    UI::clearScreen();
    UI::displayGameOver(player, cpu); // Display who won or lost
    UI::clearInputBuffer();
    std::cin.get(); // Wait for user to press Enter before returning to main menu
}

// Main menu and game flow controller
void Game::run()
{
    int choice;

    do
    {
        gameOver = false;
        UI::clearScreen();
        UI::displayMainMenu(cpuSmartMode);
        if (!(std::cin >> choice))
        {
            std::cout << "\tInvalid input. Please enter a number.\n";
            UI::clearInputBuffer();
            UI::delay(1500);
            continue;
        }

        switch (choice)
        {
        case 1:
            play(); // Start a new game
            break;
        case 2:
            UI::displayGameRules(); // Show rules
            break;
        case 3:
            std::cout << "\n\tThanks for playing Battleship, we will be waiting for your command Admiral!\n";
            break;
        case 4:
            quickplayDemo(); // Run demo mode
            break;
        case 5:
            cpuSmartMode = !cpuSmartMode; // Toggle AI mode
            std::cout << "\n\tCPU Intelligence is now set to: " << (cpuSmartMode ? "Smart" : "Normal") << "\n";
            UI::delay(1500);
            break;
        default:
            std::cout << "\n\tInvalid choice. Please enter a number between 1 and 5.\n";
            UI::delay(1500);
            break;
        }
    } while (choice != 3);
}

// Demo mode: CPU automatically destroys all player ships for demonstration
void Game::quickplayDemo()
{
    initialize();
    if (gameOver)
    {
        return;
    }

    std::vector<Position> targets;
    // Collect all positions of player's ships to target them directly
    for (const auto &ship : player.getOwnBoard().getShips())
    {
        for (const auto &pos : ship.getPositions())
        {
            targets.push_back(pos);
        }
    }

    UI::drawGameBoard(player, cpu);
    std::cout << "\n\tQuickplay Demo: CPU will now destroy all your ships!\n";
    std::cout << "\tPress Enter to continue or 'B' to go back to main menu...\n";

    std::string input;
    std::getline(std::cin, input); // Consume potential leftover newline
    std::getline(std::cin, input); // Get actual input
    if (toupper(input[0]) == 'B' && input.length() == 1)
    {
        gameOver = true;
        return;
    }

    // Iterate through all collected target positions (player ship segments)
    for (const auto &pos : targets)
    {
        if (gameOver)
            return;

        char originalCell = player.getOwnBoard().getCell(pos.x, pos.y); // Get ship type before it's marked as HIT_CHAR
        bool hit = cpu.attack(player, pos.x, pos.y);

        UI::drawGameBoard(player, cpu);
        std::cout << "\n\tCPU Quickplay attack at " << static_cast<char>('A' + pos.y) << (pos.x + 1) << ": " << (hit ? "HIT!" : "MISS!") << "\n";

        // Check if a ship part was hit and not already processed as a hit (originalCell != HIT_CHAR)
        if (hit && originalCell != HIT_CHAR)
        {
            // Find the ship that was hit to announce its destruction if it's fully sunk
            for (const auto &ship : player.getOwnBoard().getShips())
            {
                if (ship.getType() == originalCell) // Match ship type with the original cell content
                {
                    // Announce sunk ship only once per game
                    if (ship.isDestroyed() &&
                        std::find(playerShipsSunkThisGame.begin(), playerShipsSunkThisGame.end(), ship.getType()) == playerShipsSunkThisGame.end())
                    {
                        playerShipsSunkThisGame.push_back(ship.getType());
                        std::string shipName = getFullShipName(ship.getType(), false);
                        UI::displayShipDestroyed(ship.getType(), shipName, "\n\t!!! YOUR SHIP DESTROYED !!!\n\t");
                    }
                    break; // Found the ship, no need to check further
                }
            }
        }

        std::cout << "\tPress Enter to continue or 'B' to go back... \n";
        UI::clearInputBuffer();
        std::getline(std::cin, input);
        if (!input.empty() && toupper(input[0]) == 'B')
        {
            gameOver = true;
            return;
        }
    }

    gameOver = true;
    UI::clearScreen();
    UI::displayGameOver(player, cpu);
    UI::clearInputBuffer();
    std::cin.get();
}