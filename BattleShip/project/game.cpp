#include "game.h"
#include "player.h" // Assumed header for Player class
#include "board.h"  // Assumed header for Board class

#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : player("Player"), cpu("CPU"), gameOver(false), cpuSmartMode(true) {}

void Game::initialize()
{
    player.getOwnBoard().clearBoard();
    player.getTrackingBoard().clearBoard();
    cpu.getOwnBoard().clearBoard();
    cpu.getTrackingBoard().clearBoard();

    player.resetScore();
    cpu.resetScore();

    // Always place CPU ships randomly
    cpu.getOwnBoard().placeRandomShips();

    // For player ships, ask for placement method
    placePlayerShips();

    gameOver = false;
}

void Game::placePlayerShips()
{
    int choice;
    UI::displayShipPlacementMenu();
    std::cin >> choice;

    if (choice == 1)
    {
        // Manual placement
        manualShipPlacement();
    }
    else
    {
        // Random placement
        player.getOwnBoard().placeRandomShips();
        UI::displayPlayerBoard(player.getOwnBoard());
        std::cout << "\n\tShips placed randomly. Press Enter to continue...";
        UI::clearInputBuffer();
        std::cin.get();
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
        {'A', 5, "Aircraft Carrier"},
        {'B', 4, "Battleship"},
        {'C', 3, "Cruiser"},
        {'C', 3, "Cruiser"},
        {'D', 2, "Destroyer"}};

    for (const auto &def : shipDefs)
    {
        bool placed = false;
        while (!placed)
        {
            UI::displayPlayerBoard(player.getOwnBoard());
            UI::displayPlacingShip(def.name, def.length);

            Position pos = UI::getShipStartPosition();
            if (!pos.isValid())
            {
                std::cout << "\tInvalid position. Try again.\n";
                UI::delay(1000);
                continue;
            }

            Direction dir = UI::getShipDirection();
            if (static_cast<int>(dir) == -1)
            {
                std::cout << "\tInvalid direction. Try again.\n";
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
std::vector<Position> getAdjacentPositions(const Board &board, int x, int y)
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

void Game::cpuSmartTurn()
{
    // Hunt/target logic
    while (true)
    {
        int x = -1, y = -1;
        if (hunting && !huntTargets.empty())
        {
            // Continue targeting
            Position p = huntTargets.back();
            huntTargets.pop_back();
            x = p.x;
            y = p.y;
        }
        else
        {
            // Find a new random cell not tried
            do
            {
                x = rand() % BOARD_SIZE;
                y = rand() % BOARD_SIZE;
            } while (cpu.getTrackingBoard().getCell(x, y) == MISS_CHAR ||
                     cpu.getTrackingBoard().getCell(x, y) == HIT_CHAR);
        }

        std::cout << "\n\tCPU is taking aim";
        for (int i = 0; i < 3; i++)
        {
            UI::delay(300);
            std::cout << ".";
        }

        bool hit = cpu.attack(player, x, y);
        std::cout << "\n\tCPU fires at " << static_cast<char>('A' + y) << (x + 1) << ": ";
        UI::delay(700);
        UI::drawGameBoard(player, cpu);

        if (hit)
        {
            std::cout << "HIT!\n";
            lastHit = Position(x, y);
            hunting = true;
            // Add new adjacent positions to huntTargets
            for (const auto &adj : getAdjacentPositions(player.getOwnBoard(), x, y))
            {
                char cell = cpu.getTrackingBoard().getCell(adj.x, adj.y);
                if (cell != MISS_CHAR && cell != HIT_CHAR)
                {
                    // Only add if not already tried
                    bool already = false;
                    for (const auto &t : huntTargets)
                        if (t.x == adj.x && t.y == adj.y)
                            already = true;
                    if (!already)
                        huntTargets.push_back(adj);
                }
            }
            // Check if any ship was destroyed
            for (const auto &ship : player.getOwnBoard().getShips())
            {
                if (player.getOwnBoard().isShipDestroyed(ship.getType()))
                {
                    UI::displayShipDestroyed(ship.getType());
                }
            }
            if (cpu.getScore() >= winningScore)
            {
                gameOver = true;
                return;
            }
            UI::delay(1000);
            std::cout << "\n\tCPU gets another turn!\n";
            UI::delay(1000);
            continue;
        }
        else
        {
            std::cout << "MISS!\n";
            if (huntTargets.empty())
                hunting = false;
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
    }
    else
    {
        // Original random logic
        UI::displayTurnIndicator(false);
        while (true)
        {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            if (cpu.getTrackingBoard().getCell(x, y) == MISS_CHAR ||
                cpu.getTrackingBoard().getCell(x, y) == HIT_CHAR)
            {
                continue;
            }
            std::cout << "\n\tCPU is taking aim";
            for (int i = 0; i < 3; i++)
            {
                UI::delay(300);
                std::cout << ".";
            }
            bool hit = cpu.attack(player, x, y);
            std::cout << "\n\tCPU fires at " << static_cast<char>('A' + y) << (x + 1) << ": ";
            UI::delay(700);
            UI::drawGameBoard(player, cpu);
            if (hit)
            {
                std::cout << "HIT!\n";
                for (const auto &ship : player.getOwnBoard().getShips())
                {
                    if (player.getOwnBoard().isShipDestroyed(ship.getType()))
                    {
                        UI::displayShipDestroyed(ship.getType());
                    }
                }
                if (cpu.getScore() >= winningScore)
                {
                    gameOver = true;
                    return;
                }
                UI::delay(1000);
                std::cout << "\n\tCPU gets another turn!\n";
                UI::delay(1000);
                continue;
            }
            else
            {
                std::cout << "MISS!\n";
                UI::delay(1000);
                break;
            }
        }
    }
}

void Game::playerTurn()
{
    UI::displayTurnIndicator(true);

    while (true)
    {
        Position target = UI::getPlayerTarget();

        if (!target.isValid())
        {
            std::cout << "\tInvalid coordinates. Try again.\n";
            UI::delay(1000);
            continue;
        }

        // Check if already shot here
        if (player.getTrackingBoard().getCell(target.x, target.y) == MISS_CHAR ||
            player.getTrackingBoard().getCell(target.x, target.y) == HIT_CHAR)
        {
            std::cout << "\tYou already fired at this position. Try again.\n";
            UI::delay(1000);
            continue;
        }

        bool hit = player.attack(cpu, target.x, target.y);

        // Update the display
        UI::drawGameBoard(player, cpu);

        if (hit)
        {
            std::cout << "\n\tHIT at " << static_cast<char>('A' + target.y) << (target.x + 1) << "! You get another turn.\n";
            UI::delay(1000);

            // Check if any ship was completely destroyed
            for (const auto &ship : cpu.getOwnBoard().getShips())
            {
                if (cpu.getOwnBoard().isShipDestroyed(ship.getType()))
                {
                    UI::displayShipDestroyed(ship.getType());
                }
            }

            if (player.getScore() >= winningScore)
            {
                gameOver = true;
                return;
            }
        }
        else
        {
            std::cout << "\n\tMISS at " << static_cast<char>('A' + target.y) << (target.x + 1) << "! CPU's turn.\n";
            UI::delay(1000);
            break;
        }
    }
}

void Game::play()
{
    initialize();

    while (!gameOver)
    {
        UI::drawGameBoard(player, cpu);

        // Show score
        std::cout << "\n\tScore - Player: " << player.getScore() << "/17  CPU: " << cpu.getScore() << "/17\n";

        playerTurn();
        if (gameOver)
            break;

        cpuTurn();
    }

    UI::displayGameOver(player, cpu);
}

void Game::run()
{
    int choice;

    do
    {
        UI::displayMainMenu(cpuSmartMode);
        std::cin >> choice;

        switch (choice)
        {
        case 1: // Play game
            play();
            break;
        case 2: // Game rules
            UI::displayGameRules();
            break;
        case 3: // Exit
            std::cout << "\n\tThanks for playing Battleship!\n";
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
            std::cout << "\n\tInvalid choice. Please try again.\n";
            UI::delay(1500);
            break;
        }
    } while (choice != 3);
}

void Game::quickplayDemo()
{
    // CPU knows all player ship locations and fires at them in order
    initialize();
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
    UI::delay(2000);
    for (const auto &pos : targets)
    {
        cpu.attack(player, pos.x, pos.y);
        UI::drawGameBoard(player, cpu);
        UI::delay(400);
    }
    cpu.resetScore();
    for (const auto &ship : player.getOwnBoard().getShips())
        cpu.incrementScore();
    gameOver = true;
    UI::displayGameOver(player, cpu);
}