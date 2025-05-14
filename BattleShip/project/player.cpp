#include "player.h"

Player::Player(const std::string &playerName) : name(playerName), score(0) {}

const std::string &Player::getName() const { return name; }
int Player::getScore() const { return score; }
void Player::incrementScore() { score++; }
void Player::resetScore() { score = 0; }

const Board &Player::getOwnBoard() const { return ownBoard; }
const Board &Player::getTrackingBoard() const { return trackingBoard; }
Board &Player::getOwnBoard() { return ownBoard; }
Board &Player::getTrackingBoard() { return trackingBoard; }

bool Player::attack(Player &opponent, int x, int y)
{
    bool hit = opponent.getOwnBoard().processShot(x, y);

    if (hit)
    {
        trackingBoard.setCell(x, y, HIT_CHAR);
        incrementScore();
    }
    else
    {
        trackingBoard.setCell(x, y, MISS_CHAR);
    }

    return hit;
}