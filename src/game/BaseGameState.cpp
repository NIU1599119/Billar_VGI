#include "game/BaseGameState.h"

void BaseGameState::nextPlayer()
{
    m_currentPlayer++;
    if (m_currentPlayer == m_numPlayers)
    {
        m_currentPlayer = 0;
    }
}

void BaseGameState::nextTurn()
{
    m_lastPlayerCollisionPairs.clear();
    m_numTurn++;
    if (m_playerRepeatsTurn) return;

    nextPlayer();
}