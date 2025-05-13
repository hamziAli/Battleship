#include "board.h"
#include "player.h" // Include the full definition of Player
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>  // Ensure iostream is included
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

void Board::placeRandomShips()
{
    srand(static_cast<unsigned int>(time(nullptr)));

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

void UI::asciiBoat()
{
    cout << R"(
                         ~ ~                                                ~ ~
                  __/___                                                   __/___            
           _____/______|                                           _____/______|           
   _______/_____\_______\_____                               _______/_____\_______\_____     
          \              < < <       |                            \              < < <       |    
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ )"
         << endl;
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
                                     |\/                                           _  _                 |-._             
                                     ---                                        -         - _           |-._|    
                                     / | [                                     O               (). _    |
                              !      | |||                                                       '(_) __|__
                            _/|     _/|-++'                                                        [__|__|_|_]
                        +  +--|    |--|--|_ |-                                                      |__|__|_|
                     { /|__|  |/\__|  |--- |||__/                                                   |_|__|__|
                    +---------------___[}-_===_.'____                                              /|__|__|_|
                ____`-' ||___-{]_| _[}-  |     |_[___\==--                                        / |_| |___|
 __..._____--==/___]_|__|_____________________________[___\==--____,------' .7                   /  |__|__|_|
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
    asciiBoat();

    cout << "\n\n\n";
    cout << "\t\t1    2    3    4    5    6    7    8\t\t\t\t1    2    3    4    5    6    7    8" << endl;
    cout << "\t    _____________________________________________ \t\t\t    _____________________________________________     " << endl;
    cout << "\t   | ___________________________________________ |\t\t\t   | ___________________________________________ |    " << endl;
    cout << "\t   ||                                           ||\t\t\t   ||                                           ||    " << endl;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        char rowLabel = 'A' + i;
        cout << "\t " << rowLabel << " ||   ";

        // Player's board
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            cout << player.getOwnBoard().getCell(j, i) << "    ";
        }

        cout << "||\t\t\t " << rowLabel << " ||   ";

        // Tracking board (opponent's board from player's view)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            cout << player.getTrackingBoard().getCell(j, i) << "    ";
        }

        cout << "||" << endl;
        cout << "\t   ||\t\t\t\t\t\t||\t\t\t   ||\t\t\t\t\t\t||" << endl;
    }

    cout << "\t   ||___________________________________________||\t\t\t   ||___________________________________________||         " << endl;
    cout << "\t   |_____________________________________________|\t\t\t   |_____________________________________________|       " << endl;

    cout << "\n\tYour Ships\t\t\t\t\t\t\tEnemy Waters\n";
    cout << "\t(Ships: A=Carrier(5), B=Battleship(4), C=Cruiser(3), D=Destroyer(2))\t(" << HIT_CHAR << "=Hit, " << MISS_CHAR << "=Miss)\n";
}

void UI::displayGameRules()
{
    clearScreen();
    cout << "\n\n\n";
    cout << "     Get ready, for a war has been declared. Its time for some naval warfare.\n\n\n\n";
    cout << " 1.  You have to defeat your opponent by destroying all his war ships\n     before he destroys yours.\n";
    cout << " 2.  They battlefield is a 8x8 grid where your ships will be placed\n";
    cout << " 3.  You can choose to place your ships manually or randomly\n";
    cout << " 4.  To fire, enter coordinates like 'A5' (row then column)\n";
    cout << " 5.  If your attack hits the enemy ship, you get an extra turn\n";
    cout << " 6.  There are four Ship Types:\n     A Type -> Aircraft Carrier (5 cells)\n     B Type -> Battleship (4 cells)\n     C Type -> Cruiser (3 cells)\n     D Type -> Destroyer (2 cells)\n\n";
    cout << " Press Enter to continue...";
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
 / \                             \.
|   |     1. Play Battleship     |.
|   |     2. Game Rules          |.
|   |     3. Exit                |.
|   |     4. Quickplay Demo      |.
|   |     5. Toggle CPU Intelligence 
            (Current: )";
    cout << (cpuSmartMode ? "Smart" : "Normal");
    cout << R"()         |.
 \_ |                            |.
    |                            |.
    |                            |.
    |                            |.
    |                            |.
    |                            |.
    |       Enter your choice    |.
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
    cout << "\t\t===== SHIP PLACEMENT =====\n\n";
    cout << "\t\t1. Place ships manually\n";
    cout << "\t\t2. Place ships randomly\n\n";
    cout << "\t\tEnter your choice: ";
}

void UI::displayGameOver(const Player &player, const Player &cpu)
{
    clearScreen();
    drawGameBoard(player, cpu);

    cout << "\n\n\n\t\t\t\t\t\t";

    if (player.getScore() == 14 && cpu.getScore() == 14)
    {
        cout << "       ~~~ Game Has Been DRAW! ~~~~     ";
    }
    else if (cpu.getScore() == 14)
    {
        cout << "        ~~~ CPU Has Won The Game! ~~~~     ";
    }
    else
    {
        cout << "        ~~~ YOU Have Won The Game! ~~~~     ";
    }

    cout << "\n\n\t\tPress Enter to return to menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

Position UI::getPlayerTarget()
{
    string input;
    char rowInput;
    int colInput;

    cout << "\n\tEnter target coordinate (e.g. A5): ";
    cin >> input;

    if (input.length() < 2)
    {
        return Position(-1, -1); // Invalid position
    }

    rowInput = toupper(input[0]);

    // Try to extract the column number from the rest of the input
    try
    {
        colInput = stoi(input.substr(1));
    }
    catch (...)
    {
        return Position(-1, -1); // Invalid position
    }

    // Convert row character to index (A=0, B=1, etc.)
    int rowIndex = rowInput - 'A';

    // Adjust column index (user inputs 1-8, we use 0-7)
    int colIndex = colInput - 1;

    return Position(colIndex, rowIndex);
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

void UI::displayShipDestroyed(char shipType)
{
    string shipName;
    switch (shipType)
    {
    case 'A':
        shipName = "Aircraft Carrier";
        break;
    case 'B':
        shipName = "Battleship";
        break;
    case 'C':
        shipName = "Cruiser";
        break;
    case 'D':
        shipName = "Destroyer";
        break;
    default:
        shipName = "Unknown";
    }

    cout << "\n\t\t\t!!! " << shipName << " (Type " << shipType << ") has been DESTROYED! !!!\n";
    delay(1500); // Pause to emphasize the destruction
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
    char rowInput;
    int colInput;

    cout << "\tEnter starting position (e.g. A5): ";
    cin >> input;

    if (input.length() < 2)
    {
        return Position(-1, -1); // Invalid position
    }

    rowInput = toupper(input[0]);

    // Try to extract the column number from the rest of the input
    try
    {
        colInput = stoi(input.substr(1));
    }
    catch (...)
    {
        return Position(-1, -1); // Invalid position
    }

    // Convert row character to index (A=0, B=1, etc.)
    int rowIndex = rowInput - 'A';

    // Adjust column index (user inputs 1-8, we use 0-7)
    int colIndex = colInput - 1;

    return Position(colIndex, rowIndex);
}

Direction UI::getShipDirection()
{
    int dirInput;
    cout << "\tChoose direction:\n";
    cout << "\t1. Left\n";
    cout << "\t2. Right\n";
    cout << "\t3. Up\n";
    cout << "\t4. Down\n";
    cout << "\tEnter choice (1-4): ";
    cin >> dirInput;

    // Adjust input to match Direction enum (0-3)
    dirInput = (dirInput >= 1 && dirInput <= 4) ? dirInput - 1 : -1;

    if (dirInput < 0 || dirInput > 3)
    {
        return static_cast<Direction>(-1); // Invalid direction
    }

    return static_cast<Direction>(dirInput);
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
    cout << "\n\t(Ships: A=Carrier(5), B=Battleship(4), C=Cruiser(3), D=Destroyer(2))\n";
}
