#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <vector>
#include "board.h"

// Player class
class Player
{
private:
    std::string name;
    Board ownBoard;
    Board trackingBoard; // To track shots against opponent
    int score;

public:
    Player(const std::string &playerName);

    const std::string &getName() const;
    int getScore() const;
    void incrementScore();
    void resetScore();

    const Board &getOwnBoard() const;
    const Board &getTrackingBoard() const;
    Board &getOwnBoard();
    Board &getTrackingBoard();

    bool attack(Player &opponent, int x, int y);
};

#endif