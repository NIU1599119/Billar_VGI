#pragma once
#include "debug.h"
#include <vector>
#include "game/gamemodes.h"

struct intPair
{
    intPair(int l, int r) : left(l), right(r) {};
    int left;
    int right;
};

class BaseGameState
{
public:
    BaseGameState(int numPlayers, int numBalls, GAMEMODE gamemode) : m_numPlayers(numPlayers), m_numBalls(numBalls), m_gamemode(gamemode) {};
    ~BaseGameState() {};

    int getCurrentPlayer() { return m_currentPlayer; };

    // set the current player to the next player
    void nextPlayer();

    // passes the turn and resets player variables, then calls next player if m_playerRepeatsTurn
    void nextTurn();

    // virtual function so that the children can reset player variables before the nextTurn, m_playerRepeatsTurn should be set here
    virtual void processTurn() { nextTurn(); m_playerRepeatsTurn = false; };

    GAMEMODE getGamemode() { return m_gamemode; };

    void registerCollision(int ball1, int ball2) { m_lastPlayerCollisionPairs.push_back(intPair(ball1, ball2)); };

    virtual int getPlayerBallID() { return 0; };

protected:
    GAMEMODE m_gamemode;
    int m_numPlayers;
    int m_currentPlayer = 0;

    int m_numTurn = 0;

    int m_numBalls;
    std::vector<intPair> m_lastPlayerCollisionPairs;  // pairs of ball colisions in a player's turn, resets in player change

    bool m_playerRepeatsTurn = false;
};
