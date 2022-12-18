#pragma once
#include "game/BaseGameState.h"

static const int N_CARAMBOLA_BALLS = 3;

class CarambolaState : public BaseGameState
{
public:
    CarambolaState(int numPlayers) : BaseGameState(numPlayers, N_CARAMBOLA_BALLS, CARAMBOLA) {};

    int getPlayerBallID() { return 1; };

private:

};
