#pragma once
#include "game/BaseGameState.h"

enum CLASSIC_BALL_TYPES
{
    CUE,
    SOLID,
    STRIPED,
    EIGHT
};


class ClassicState : public BaseGameState
{
public:
    ClassicState(int numPlayers) : BaseGameState(numPlayers, N_CLASSIC_BALLS, CLASSIC), m_playerTeam(numPlayers, -1), m_teams(N_TEAMS, CUE), m_pocketedBalls(N_CLASSIC_BALLS, false) {
        ASSERT(numPlayers % N_TEAMS == 0);  // 1v1, 2v2, 3v3

        for (int i = 0; i < numPlayers; i++)
        {
            m_playerTeam[i] = i % N_TEAMS;
        }

    };

    void pocketBall(int ballID);

    // returns if the current player has to repeat turn
    bool willRepeatTurn();

    // returns -1 if game is not over, or the team id of the team that won
    int gameIsOver();

    int getPlayerBallID() { return 0; };

private:
    // team info
    static const int N_TEAMS = 2;   // number of teams
    std::vector<CLASSIC_BALL_TYPES> m_teams;    // saves the type of balls that the team has to pocket

    std::vector<int> m_playerTeam;  // members of teams

    static const int N_CLASSIC_BALLS = 16;  // 1 CUE, 7 SOLID, 1 EIGHT, 7 STRIPED
    const std::vector<CLASSIC_BALL_TYPES> BALL_TYPES = { CUE, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, SOLID, EIGHT, STRIPED, STRIPED, STRIPED, STRIPED, STRIPED, STRIPED, STRIPED };
    std::vector<bool> m_pocketedBalls;

    // checks if the team of the current player wins the game or not
    bool checkWin();
    int m_winningTeam = -1;

    // current player info
    bool m_playerFoul = false;          // skipear el turno (bola blanca se resetearia)
    bool m_repeatPlayerTurn = false;    // repetir turno (se invalida si se skipea el turno)
};
