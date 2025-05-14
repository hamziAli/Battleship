#include "game.h"
#include "player.h" // Assumed header for Player class
#include "board.h"  // Assumed header for Board class

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string> // Added for std::to_string
#include <vector>
#include <algorithm> // For std::find

using namespace std; // Add namespace usage

Game::Game() : player("Player"), cpu("CPU"), gameOver(false), cpuSmartMode(true), hunting(false) {}

// Helper function to get full ship names
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

    cpu.getOwnBoard().placeRandomShips(true); // true for enemy board
    placePlayerShips();
    if (gameOver)
        return;

    gameOver = false;
}

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
            player.getOwnBoard().placeRandomShips(false); // false for player board
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

void Game::manualShipPlacement()
{
    // Define ship types and lengths
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

    for (const auto &def : shipDefs)
    {
        bool placed = false;
        while (!placed)
        {
            UI::displayPlayerBoard(player.getOwnBoard());
            UI::displayPlacingShip(def.name, def.length);
            Position pos = UI::getShipStartPosition();
            if (pos.x == -2 && pos.y == -2)
            {
                gameOver = true;
                return;
            }
            if (!pos.isValid())
            {
                std::cout << "\tInvalid position. Try again.\n";
                UI::delay(1000);
                continue;
            }
            Direction dir = UI::getShipDirection();
            if (static_cast<int>(dir) == -2)
            {
                gameOver = true;
                return;
            }
            // Assuming Direction enum is 0-3 for valid, and -2 for back.
            if (static_cast<int>(dir) < 0 || static_cast<int>(dir) > 3)
            {
                std::cout << "\tInvalid direction selection. Try again.\n";
                UI::delay(1000);
                continue;
            }
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

// Helper: Get valid adjacent positions
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

void Game::playerTurn()
{
    UI::displayTurnIndicator(true);
    while (true)
    {
        Position target = UI::getPlayerTarget();
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
            for (const auto &ship : cpu.getOwnBoard().getShips())
            {
                if (ship.isDestroyed() &&
                    std::find(cpuShipsSunkThisGame.begin(), cpuShipsSunkThisGame.end(), ship.getType()) == cpuShipsSunkThisGame.end())
                {
                    cpuShipsSunkThisGame.push_back(ship.getType());
                    std::string shipName = getFullShipName(ship.getType(), true); // true for enemy
                    UI::displayShipDestroyed(ship.getType(), shipName, "\n\t!!! ENEMY SHIP DESTROYED !!!\n\t");
                }
            }
            if (player.getScore() >= winningScore)
            {
                gameOver = true;
                return;
            }
            std::cout << "\n\tYou get another turn!\n";
            UI::delay(1000);
        }
        else
        {
            std::cout << "MISS!\n";
            UI::delay(1000);
            break;
        }
    }
}

void Game::cpuTurn()
{
    if (cpuSmartMode)
    {
        cpuSmartTurn();
        return;
    }

    UI::displayTurnIndicator(false);
    while (true)
    {
        int x, y;
        do
        {
            x = rand() % BOARD_SIZE;
            y = rand() % BOARD_SIZE;
        } while (cpu.getTrackingBoard().getCell(x, y) != EMPTY_CHAR &&
                 cpu.getTrackingBoard().getCell(x, y) != WATER_CHAR);

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
            for (const auto &ship : player.getOwnBoard().getShips())
            {
                if (ship.isDestroyed() &&
                    std::find(playerShipsSunkThisGame.begin(), playerShipsSunkThisGame.end(), ship.getType()) == playerShipsSunkThisGame.end())
                {
                    playerShipsSunkThisGame.push_back(ship.getType());
                    std::string shipName = getFullShipName(ship.getType(), false); // false for player
                    UI::displayShipDestroyed(ship.getType(), shipName, "\n\t!!! YOUR SHIP DESTROYED !!!\n\t");
                }
            }
            if (cpu.getScore() >= winningScore)
            {
                gameOver = true;
                return;
            }
            std::cout << "\n\tCPU gets another turn!\n";
            UI::delay(1000);
        }
        else
        {
            std::cout << "MISS!\n";
            UI::delay(1000);
            break;
        }
    }
}

void Game::cpuSmartTurn()
{
    UI::displayTurnIndicator(false);
    while (true)
    {
        int x = -1, y = -1;
        if (hunting && !huntTargets.empty())
        {
            Position p = huntTargets.back();
            huntTargets.pop_back();
            x = p.x;
            y = p.y;
            if (cpu.getTrackingBoard().getCell(x, y) != EMPTY_CHAR &&
                cpu.getTrackingBoard().getCell(x, y) != WATER_CHAR)
            {
                continue;
            }
        }
        else
        {
            hunting = false;
            do
            {
                x = rand() % BOARD_SIZE;
                y = rand() % BOARD_SIZE;
            } while (cpu.getTrackingBoard().getCell(x, y) != EMPTY_CHAR &&
                     cpu.getTrackingBoard().getCell(x, y) != WATER_CHAR);
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
            lastHit = Position(x, y);
            hunting = true;
            std::vector<Position> potentialTargets = getAdjacentPositions(player.getOwnBoard(), x, y);
            for (const auto &pt : potentialTargets)
            {
                if (cpu.getTrackingBoard().getCell(pt.x, pt.y) == EMPTY_CHAR ||
                    cpu.getTrackingBoard().getCell(pt.x, pt.y) == WATER_CHAR)
                { // only add if not shot before
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

            for (const auto &ship : player.getOwnBoard().getShips())
            {
                if (ship.isDestroyed() &&
                    std::find(playerShipsSunkThisGame.begin(), playerShipsSunkThisGame.end(), ship.getType()) == playerShipsSunkThisGame.end())
                {
                    playerShipsSunkThisGame.push_back(ship.getType());
                    std::string shipName = getFullShipName(ship.getType(), false); // false for player
                    UI::displayShipDestroyed(ship.getType(), shipName, "\n\t!!! YOUR SHIP DESTROYED !!!\n\t");
                }
            }
            if (cpu.getScore() >= winningScore)
            {
                gameOver = true;
                return;
            }
            std::cout << "\n\tCPU gets another turn!\n";
            UI::delay(1000);
        }
        else
        {
            std::cout << "MISS!\n";
            UI::delay(1000);
            break;
        }
    }
}

void Game::play()
{
    initialize();
    if (gameOver)
    { // If initialize() set gameOver (back was pressed), return immediately
        return;
    }

    while (!gameOver)
    {
        UI::drawGameBoard(player, cpu);

        // Show score
        std::cout << "\n\tScore - Player: " << player.getScore() << "/17  CPU: " << cpu.getScore() << "/17\n";

        playerTurn();
        if (gameOver)
            return; // Return immediately if back was pressed

        cpuTurn();
        if (gameOver)
            return; // Return immediately if game is over
    }

    UI::clearScreen();
    UI::displayGameOver(player, cpu);
    UI::clearInputBuffer();
    std::cin.get(); // Wait for user input before returning to menu
}

void Game::run()
{
    int choice;

    do
    {
        gameOver = false;  // Reset gameOver at start of each menu iteration
        UI::clearScreen(); // Keep this one for menu transitions
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
        case 1: // Play game
            play();
            break;
        case 2: // Game rules
            UI::displayGameRules();
            break;
        case 3: // Exit
            std::cout << "\n\tThanks for playing Battleship, we will be waiting for your command Admiral!\n";
            break;
        case 4: // Quickplay Demo
            quickplayDemo();
            break;
        case 5: // Toggle CPU intelligence
            cpuSmartMode = !cpuSmartMode;
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

void Game::quickplayDemo()
{
    initialize();
    if (gameOver)
    { // If initialize() set gameOver (back was pressed), return immediately
        return;
    }

    std::vector<Position> targets;
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
    std::getline(std::cin, input); // Clear any previous input
    std::getline(std::cin, input); // Get actual input

    if (toupper(input[0]) == 'B' && input.length() == 1)
    {
        gameOver = true;
        return;
    }

    for (const auto &pos : targets)
    {
        if (gameOver)
            return; // Check for back command after each shot

        char originalCell = player.getOwnBoard().getCell(pos.x, pos.y);
        bool hit = cpu.attack(player, pos.x, pos.y); // CPU attacks Player's board

        UI::drawGameBoard(player, cpu); // Redraw to show hit
        std::cout << "\n\tCPU Quickplay attack at " << static_cast<char>('A' + pos.y) << (pos.x + 1) << ": " << (hit ? "HIT!" : "MISS!") << "\n";

        if (hit && originalCell != HIT_CHAR)
        { // Check if it was a ship cell and not already processed as part of a sunk ship
            for (const auto &ship : player.getOwnBoard().getShips())
            {
                if (ship.getType() == originalCell)
                { // Found the ship that was hit
                    if (ship.isDestroyed() &&
                        std::find(playerShipsSunkThisGame.begin(), playerShipsSunkThisGame.end(), ship.getType()) == playerShipsSunkThisGame.end())
                    {
                        playerShipsSunkThisGame.push_back(ship.getType());
                        std::string shipName = getFullShipName(ship.getType(), false);
                        UI::displayShipDestroyed(ship.getType(), shipName, "\n\t!!! YOUR SHIP DESTROYED !!!\n\t");
                    }
                    break;
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
    std::cin.get(); // Wait for user input before returning to menu
}