#pragma once
#include "entities/Entity.h"
#include <string>

const double MAX_BALL_FORCE = 4.0;

namespace Entities
{
    class EntityBall : public Entity
    {
    public:
        EntityBall(int ballID, BaseGameState* gamestate) : Entity(BALL, gamestate), m_ballID(ballID) {};

        int getBallID() { return m_ballID; };

        void collision(Entity* entity, double force);

        std::string getBallName();

    private:
        int m_ballID;

    };


};
