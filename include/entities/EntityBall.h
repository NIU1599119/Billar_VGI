#pragma once
#include "entities/Entity.h"
#include <string>

namespace Entities
{
    class EntityBall : public Entity
    {
    public:
        EntityBall(int ballID, GAME_TYPE game_type) : Entity(BALL, game_type), m_ballID(ballID) {};

        int getBallID() { return m_ballID; };

        void collision(Entity* entity);

        std::string getBallName();

    private:
        int m_ballID;
    };


};
