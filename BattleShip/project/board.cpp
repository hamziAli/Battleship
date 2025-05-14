#include "board.h"
#include "player.h" // Include the full definition of Player
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream> // Ensure iostream is included
#include <vector>
#include <algorithm> // For std::find if used later, not strictly for this step
using namespace std; // Use the standard namespace for cin and endl

#undef BOARD_SIZE

// Position implementations
Position::Position() : x(0), y(0) {}
Position::Position(int xPos, int yPos) : x(xPos), y(yPos) {}

bool Position::isValid() const
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool Position::operator==(const Position &other) const
{
    return x == other.x && y == other.y;
}

// Ship implementations
Ship::Ship(char shipType, int shipLength) : type(shipType), length(shipLength), hitsRemaining(shipLength) {}

char Ship::getType() const { return type; }
int Ship::getLength() const { return length; }
int Ship::getHitsRemaining() const { return hitsRemaining; }

bool Ship::isDestroyed() const { return hitsRemaining <= 0; }

void Ship::hit()
{
    if (hitsRemaining > 0)
    {
        hitsRemaining--;
    }
}

void Ship::addPosition(const Position &pos)
{
    positions.push_back(pos);
}

const std::vector<Position> &Ship::getPositions() const
{
    return positions;
}

// Board implementations
Board::Board()
{
    clearBoard();
}

void Board::clearBoard()
{
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            grid[y][x] = EMPTY_CHAR;
        }
    }
    ships.clear();
}

char Board::getCell(int x, int y) const
{
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE)
    {
        return grid[y][x];
    }
    return ' ';
}

void Board::setCell(int x, int y, char value)
{
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE)
    {
        grid[y][x] = value;
    }
}

bool Board::isEmptyCell(int x, int y) const
{
    return getCell(x, y) == EMPTY_CHAR;
}

bool Board::isValidPlacement(int x, int y, int length, Direction dir) const
{
    switch (dir)
    {
    case LEFT:
        if (x - length + 1 < 0)
            return false;
        for (int i = 0; i < length; i++)
        {
            if (!isEmptyCell(x - i, y))
                return false;
        }
        break;
    case RIGHT:
        if (x + length > BOARD_SIZE)
            return false;
        for (int i = 0; i < length; i++)
        {
            if (!isEmptyCell(x + i, y))
                return false;
        }
        break;
    case UP:
        if (y - length + 1 < 0)
            return false;
        for (int i = 0; i < length; i++)
        {
            if (!isEmptyCell(x, y - i))
                return false;
        }
        break;
    case DOWN:
        if (y + length > BOARD_SIZE)
            return false;
        for (int i = 0; i < length; i++)
        {
            if (!isEmptyCell(x, y + i))
                return false;
        }
        break;
    }
    return true;
}

void Board::placeShip(Ship &ship, int x, int y, Direction dir)
{
    int length = ship.getLength();

    switch (dir)
    {
    case LEFT:
        for (int i = 0; i < length; i++)
        {
            grid[y][x - i] = ship.getType();
            ship.addPosition(Position(x - i, y));
        }
        break;
    case RIGHT:
        for (int i = 0; i < length; i++)
        {
            grid[y][x + i] = ship.getType();
            ship.addPosition(Position(x + i, y));
        }
        break;
    case UP:
        for (int i = 0; i < length; i++)
        {
            grid[y - i][x] = ship.getType();
            ship.addPosition(Position(x, y - i));
        }
        break;
    case DOWN:
        for (int i = 0; i < length; i++)
        {
            grid[y + i][x] = ship.getType();
            ship.addPosition(Position(x, y + i));
        }
        break;
    }

    ships.push_back(ship);
}

void Board::placeRandomShips(bool isEnemyBoard)
{
    // Seed random number generator, add offset for enemy to ensure different layouts if called quickly
    srand(static_cast<unsigned int>(time(nullptr)) + (isEnemyBoard ? 100 : 0));

    struct ShipDef
    {
        char type;
        int length;
        // Name is not strictly needed here for placement, Game logic will handle full names for display
    };

    std::vector<ShipDef> shipsToPlace;

    if (isEnemyBoard)
    {
        shipsToPlace = {
            {'A', 5}, // Alpha
            {'B', 4}, // Bravo
            {'C', 3}, // Charlie
            {'D', 3}, // Delta
            {'E', 2}  // Echo
        };
    }
    else
    {
        shipsToPlace = {
            {'T', 5}, // PNS Tughril
            {'Z', 4}, // PNS Zulfiqar
            {'H', 3}, // PNS Hangor
            {'Y', 3}, // PNS Yarmuk
            {'M', 2}  // PNS Mujahid
        };
    }

    for (const auto &def : shipsToPlace)
    {
        bool placed = false;
        while (!placed)
        {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            Direction dir = static_cast<Direction>(rand() % 4);

            if (isValidPlacement(x, y, def.length, dir))
            {
                Ship ship(def.type, def.length);
                placeShip(ship, x, y, dir);
                placed = true;
            }
        }
    }
}

bool Board::processShot(int x, int y)
{
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
    {
        return false;
    }

    char cell = grid[y][x];

    // Already shot here
    if (cell == MISS_CHAR || cell == HIT_CHAR)
    {
        return false;
    }

    // Miss
    if (cell == EMPTY_CHAR)
    {
        grid[y][x] = MISS_CHAR;
        return false;
    }

    // Hit a ship
    char shipType = cell;
    grid[y][x] = HIT_CHAR;

    // Find and update the ship
    for (auto &ship : ships)
    {
        if (ship.getType() == shipType)
        {
            ship.hit();
            break;
        }
    }

    return true;
}

bool Board::allShipsDestroyed() const
{
    for (const auto &ship : ships)
    {
        if (!ship.isDestroyed())
        {
            return false;
        }
    }
    return !ships.empty();
}

bool Board::isShipDestroyed(char shipType) const
{
    for (const auto &ship : ships)
    {
        if (ship.getType() == shipType && ship.isDestroyed())
        {
            return true;
        }
    }
    return false;
}

const std::vector<Ship> &Board::getShips() const
{
    return ships;
}

void UI::clearScreen()
{
    // Use ANSI escape code to clear screen
    cout << "\033[2J\033[1;1H";
}

void UI::delay(int milliseconds)
{
    // Add delay for more realistic gameplay
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

void UI::Battleshiplogo()
{
    cout << "\n\n\n";
    cout << " _           _   _   _           _     _       " << endl;
    cout << "| |         | | | | | |         | |   (_)      " << endl;
    cout << "| |__   __ _| |_| |_| | ___  ___| |__  _ _ __  " << endl;
    cout << "| '_ \\ / _` | __| __| |/ _ \\/ __| '_ \\| | '_ \\ " << endl;
    cout << "| |_) | (_| | |_| |_| |  __/\\__ \\ | | | | |_) |" << endl;
    cout << "|_.__/ \\__,_|\\__|\\__|_|\\___||___/_| |_|_| .__/ " << endl;
    cout << "                                        | |    " << endl;
    cout << "                                        |_|    " << endl;
}

void UI::Battleshiplogo2()
{
    cout << R"(               
    
                                     _________   
                                     | |::::::| 
                                     | |:(_*::| 
                                     |_|::::::| 
                                     |\/                                           _  _                 |-._             
                                     ---                                        -         - _           |-._|    
                                     / | [                                     O               (). _    |
                              !      | |||                                                       '(_) __|__
                            _/|     _/|-++'                                                        [__|__|_|_]
                        +  +--|    |--|--|_ |-                                                      |__|__|_|
                     { /|__|  |/\__|  |--- |||__/                                                   |_|__|__| 
                    +---------------___[}-_===_.'____                                              /|__|__|_|
                ____`-' ||___-{]_| _[}-  |     |_[___']==--                                       / |_| |___|
 __..._____--==/___]_|__|_____________________________[___']==--____,------' .7                  /  |__|__|_|
|                   Made by: 2024302, 2024208, 2024532                      /                   /   |__|__|_|
 \_________________________________________________________________________|                   /    |__|__|_|
 wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
   wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
 )" << endl;
}

void UI::loadingEffect(const string &message, int dotCount, int delayMs)
{
    // Left-align the loading message
    cout << message;
    for (int i = 0; i < dotCount; ++i)
    {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
    cout << endl;
}

void UI::spinner(int durationMs)
{
    const char spinnerChars[] = {'|', '/', '-', '\\'};
    int steps = durationMs / 100;
    string prefix = "Loading ";
    // Print initial spinner position
    cout << prefix << spinnerChars[0] << flush;
    for (int i = 1; i < steps; ++i)
    {
        cout << "\r" << prefix << spinnerChars[i % 4] << flush;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    cout << "\r" << prefix << "done!   " << endl;
}

void UI::drawGameBoard(const Player &player, const Player &opponent)
{
    cout << "\n\n\n";
    cout << "\n\n\n";
    cout << "\t\t1    2    3    4    5    6    7    8\t\t\t\t         1    2    3    4    5    6    7    8" << endl;
    cout << "\t    _____________________________________________ \t\t\t    _____________________________________________     " << endl;
    cout << "\t   | ___________________________________________ |\t\t\t   | ___________________________________________ |    " << endl;
    cout << "\t   ||                                           ||\t\t\t   ||                                           ||    " << endl;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        char rowLabel = 'A' + i;
        cout << "\t " << rowLabel << " ||   ";
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            cout << player.getOwnBoard().getCell(j, i) << "    ";
        }
        cout << "||\t\t\t " << rowLabel << " ||   ";
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            cout << player.getTrackingBoard().getCell(j, i) << "    ";
        }
        cout << "||" << endl;
        cout << "\t   ||\t\t\t\t\t\t||\t\t\t   ||\t\t\t\t\t\t||" << endl;
    }

    cout << "\t   ||___________________________________________||\t\t\t   ||___________________________________________||         " << endl;
    cout << "\t   |_____________________________________________|\t\t\t   |_____________________________________________|       " << endl;
    cout << "\n\t\t\tYour Ships\t\t\t\t\t\t\t\t\tEnemy Waters\n";
    cout << "\n";
    cout << "\t\t\t(Player: T=Tughril(5), Z=Zulfiqar(4), H=Hangor(3), Y=Yarmuk(3), M=Mujahid(2))\n";                                                     // Clarified Player ships
    cout << "\t\t\t(Enemy:  A=Alpha(5),   B=Bravo(4),   C=Charlie(3), D=Delta(3),   E=Echo(2))\t\t(" << HIT_CHAR << "=Hit, " << MISS_CHAR << "=Miss)\n"; // Clarified Enemy ships
}

void UI::displayGameRules()
{
    clearScreen();
    cout << "\n\n\n";
    cout << "     Get ready Admiral! Pakistan Navy calls you to an epic Battleship showdown!\n\n\n\n";
    cout << "Remember, ships are safe at the harbour but that is not what they are built for!\n";
    cout << " 1.  Outsmart your rival by sinking their entire fleet before they sink yours!\n";
    cout << " 2.  Navigate a strategic 8x8 ocean grid to position your naval forces.\n";
    cout << " 3.  Place your ships with cunning precision or let the tides decide with random deployment.\n";
    cout << " 4.  Fire your salvo by targeting coordinates like 'A5' (row, then column).\n";
    cout << " 5.  Score a direct hit on an enemy ship, and you'll earn an extra shot to rule the waves!\n";
    cout << " 6.  Command Pakistan Navy's legendary vessels:\n";
    cout << "     T Type -> PNS Tughril, the towering frigate (5x1 cells)\n";
    cout << "     Z Type -> PNS Zulfiqar, the striking sword of the sea (4x1 cells)\n";
    cout << "     H Type -> PNS Hangor, the silent predator (3x1 cells)\n";
    cout << "     Y Type -> PNS Yarmuk, steadfast defender (3x1 cells)\n"; // Typo fix
    cout << "     M Type -> PNS Mujahid, bold striker (2x1 cells)\n\n";    // Typo fix
    cout << " Press Enter to steer Pakistan Navy to victory in Battleship...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void UI::displayMainMenu(bool cpuSmartMode)
{
    clearScreen();
    Battleshiplogo();
    Battleshiplogo2();
    cout << "\n\n";
    UI::loadingEffect("Sink or be sunk", 3, 500);
    UI::spinner(3000);
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "\n\n";
    cout << R"(   ______________________________
 / \        ~~ MAIN MENU ~~      \.
|   |    1. Play Battleship      |.
|   |    2. Game Rules           |.
|   |    3. Exit                 |.
|   |    4. Quickplay Demo       |.
|   |    5. Toggle CPU Intelligence (Current: )"
         << (cpuSmartMode ? "Smart" : "Normal") << R"()                 |.
 \_ |                            |.
    |                            |.
    |                            |.
    |                            |.
    |                            |.
    |      Enter your choice     |.
    |                            |.
    |   _________________________|___
    |  /                            /.
    \_/____________________________/.
    )" << endl;
}

void UI::displayShipPlacementMenu()
{
    clearScreen();
    cout << "\n\n";
    cout << R"(   ______________________________
 / \      ~~ SHIP PLACEMENT ~~   \.
|   |     1. Place Randomly      |.
|   |     2. Place Manually      |.
|   |     3. Back to Main Menu   |.
|   |                            |.
|   |                            |.
|   |                            |.
 \_ |                            |.
    |                            |.
    |                            |.
    |                            |.
    |                            |.
    |                            |.
    |      Enter your choice     |.
    |                            |.
    |   _________________________|___
    |  /                            /.
    \_/____________________________/.
    )" << endl;
}

void UI::displayGameOver(const Player &player, const Player &cpu)
{
    clearScreen();
    drawGameBoard(player, cpu);

    cout << "\n\n\n\t\t\t\t\t\t";

    if (cpu.getScore() == 17)
    {
        cout << R"(

        CPU HAS WON THE GAME!

        )";
    }
    else
    {
        cout << R"(

        YOU HAVE WON THE GAME!

        )";
    }

    cout << "\n\n\t\tPress Enter to return to menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

Position UI::getPlayerTarget()
{
    string input;
    while (true)
    {
        cout << "\n\tEnter target coordinate (e.g. A5) or 'B' to go back: ";
        cin >> input;

        if (toupper(input[0]) == 'B' && input.length() == 1)
        {
            return Position(-2, -2); // Special value for back
        }

        if (input.length() < 2)
        {
            cout << "\tInvalid coordinate format. Try again.\n";
            continue;
        }

        char rowInput = toupper(input[0]);
        if (rowInput < 'A' || rowInput > 'H')
        {
            cout << "\tInvalid row. Use letters A-H.\n";
            continue;
        }

        try
        {
            int colInput = stoi(input.substr(1));
            if (colInput < 1 || colInput > 8)
            {
                cout << "\tInvalid column. Use numbers 1-8.\n";
                continue;
            }

            // Convert row character to index (A=0, B=1, etc.)
            int rowIndex = rowInput - 'A';
            // Adjust column index (user inputs 1-8, we use 0-7)
            int colIndex = colInput - 1;

            return Position(colIndex, rowIndex);
        }
        catch (...)
        {
            cout << "\tInvalid column number. Try again.\n";
            continue;
        }
    }
}

void UI::displayTurnIndicator(bool playerTurn)
{
    if (playerTurn)
    {
        cout << "\n\t\t\t\t\t*** YOUR TURN ***\n";
    }
    else
    {
        cout << "\n\t\t\t\t\t*** CPU TURN ***\n";
    }
}

void UI::displayShipDestroyed(char shipType, const std::string &fullShipName, const std::string &contextPrefix)
{
    // contextPrefix already includes newlines and formatting like "\n\t!!! ENEMY SHIP DESTROYED !!!\n\t"
    cout << contextPrefix << fullShipName << " (Type " << shipType << ") has been DESTROYED!" << endl;
    delay(2000); // Pause to emphasize the destruction
}

void UI::clearInputBuffer()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void UI::displayPlacingShip(const string &shipName, int length)
{
    cout << "\n\tPlacing " << shipName << " (Length: " << length << ")\n";
}

Position UI::getShipStartPosition()
{
    string input;
    while (true)
    {
        cout << "\tEnter starting position (e.g. A5) or 'B' to go back: ";
        cin >> input;

        if (toupper(input[0]) == 'B' && input.length() == 1)
        {
            return Position(-2, -2); // Special value for back
        }

        if (input.length() < 2)
        {
            cout << "\tInvalid position format. Try again.\n";
            continue;
        }

        char rowInput = toupper(input[0]);
        if (rowInput < 'A' || rowInput > 'H')
        {
            cout << "\tInvalid row. Use letters A-H.\n";
            continue;
        }

        try
        {
            int colInput = stoi(input.substr(1));
            if (colInput < 1 || colInput > 8)
            {
                cout << "\tInvalid column. Use numbers 1-8.\n";
                continue;
            }

            // Convert row character to index (A=0, B=1, etc.)
            int rowIndex = rowInput - 'A';
            // Adjust column index (user inputs 1-8, we use 0-7)
            int colIndex = colInput - 1;

            return Position(colIndex, rowIndex);
        }
        catch (...)
        {
            cout << "\tInvalid column number. Try again.\n";
            continue;
        }
    }
}

Direction UI::getShipDirection()
{
    int dirInput;
    while (true)
    {
        cout << "\tChoose direction:\n";
        cout << "\t1. Left\n";
        cout << "\t2. Right\n";
        cout << "\t3. Up\n";
        cout << "\t4. Down\n";
        cout << "\t5. Back\n";
        cout << "\tEnter choice (1-5): ";

        if (!(cin >> dirInput))
        {
            cout << "\tInvalid input. Please enter a number.\n";
            clearInputBuffer();
            continue;
        }

        if (dirInput == 5)
        {
            return static_cast<Direction>(-2); // Special value for back
        }

        if (dirInput >= 1 && dirInput <= 4)
        {
            return static_cast<Direction>(dirInput - 1);
        }

        cout << "\tInvalid choice. Please enter a number between 1 and 5.\n";
        clearInputBuffer();
    }
}

void UI::displayPlayerBoard(const Board &board)
{
    clearScreen();
    cout << "\n\n\t\tYOUR SHIP PLACEMENT\n\n";
    cout << "\t\t  1  2  3  4  5  6  7  8\n";
    cout << "\t\t  -----------------------\n";

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        char rowLabel = 'A' + i;
        cout << "\t\t" << rowLabel << "|";

        for (int j = 0; j < BOARD_SIZE; j++)
        {
            cout << " " << board.getCell(j, i) << " ";
        }

        cout << "|\n";
    }

    cout << "\t\t  -----------------------\n";
    cout << "\n\t(Ships: T=Tughril(5), Z=Zulfiqar(4), H=Hangor(3), Y=Yarmuk(3), M=Mujahid(2))\n";
}
