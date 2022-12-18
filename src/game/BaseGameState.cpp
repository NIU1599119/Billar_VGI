#include "game/BaseGameState.h"

void BaseGameState::nextPlayer()
{
    m_currentPlayer++;
    if (m_currentPlayer == m_numPlayers)
    {
        m_currentPlayer = 0;
        m_numTurn++;
    }

    m_lastPlayerCollisionPairs.clear();
}
