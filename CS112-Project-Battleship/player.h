#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <vector>
#include "board.h"

class Player
{
private:
    std::string name;
    Board ownBoard;
    Board trackingBoard; // To track shots against opponent
    int score;

public:
    Player(const std::string &playerName); // Access player name

    const std::string &getName() const;
    int getScore() const;
    void incrementScore(); // Increase score by one
    void resetScore();

    const Board &getOwnBoard() const;
    const Board &getTrackingBoard() const;
    Board &getOwnBoard(); // Modify own board
    Board &getTrackingBoard();
    // Perform attack on opponent at (x, y); returns true if hit
    bool attack(Player &opponent, int x, int y);
};

#endif