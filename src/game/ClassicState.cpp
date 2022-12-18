#include "game/ClassicState.h"

void ClassicState::pocketBall(int ballID)
{
    // checkeamos si el equipo tiene una bola asignada o no. (tambien que lo que hayamos metido sean lisas o ralladas, si no pues no se asigna todavia)
    if (m_teams[m_playerTeam[m_currentPlayer]] == CUE && (BALL_TYPES[ballID] == SOLID || BALL_TYPES[ballID] == STRIPED))
    {
        m_teams[m_playerTeam[m_currentPlayer]] = BALL_TYPES[ballID];

        if (N_TEAMS == 2)
        {
            int otherTeam = (m_playerTeam[m_currentPlayer]+1)%N_TEAMS;
            m_teams[otherTeam] = (BALL_TYPES[ballID] == SOLID) ? STRIPED : SOLID;
        }
        else
        {
            LOG_ERROR("No classic game for %d teams", N_TEAMS);
            LOG_ERROR("Classic mode is programmed for 2 teams only");
            ASSERT(false);
        }
    }

    // segun el tipo de bolas metidas hacemos una cosa o otra
    switch (BALL_TYPES[ballID])
    {
    case CUE:
        m_playerFoul = true;    // invalida el repetir turno y simplemente lo salta
        // avisar de que el proximo jugador coloque la bola blanca (o que se coloque sola)
        break;
    case EIGHT:
    {
        // checkear si se gana o se pierde el juego
        m_pocketedBalls[ballID] = true;
        bool teamWin = checkWin();
        if (teamWin)
        {
            // team wins
            m_winningTeam = m_playerTeam[m_currentPlayer];
        }
        else
        {
            // team loses, the other team wins
            m_winningTeam = (m_playerTeam[m_currentPlayer]+1)%2;
        }
        break;
    }
    default:    // SOLID, STRIPED  (lisas o ralladas)
        m_pocketedBalls[ballID] = true;
        if (m_teams[m_playerTeam[m_currentPlayer]] == BALL_TYPES[ballID])  // si ha metido bola y es la que tiene que meter
        {
            // el jugador puede repetir el turno
            m_repeatPlayerTurn = true;
        }
        // si mete bola contraria no pasa nada creo
        break;
    }
}

bool ClassicState::checkWin()
{
    bool teamWin = true;
    CLASSIC_BALL_TYPES currentPlayerBallType = m_teams[m_playerTeam[m_currentPlayer]];
    for (int i = 0; i < N_CLASSIC_BALLS; i++)
    {
        if (BALL_TYPES[i] == currentPlayerBallType)
        {
            teamWin = m_pocketedBalls[i];   // if ball is not pocketed then game is not won
            if (!teamWin) break;
        }
    }
    return teamWin;
}