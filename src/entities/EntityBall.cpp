#include "entities/EntityBall.h"

namespace Entities
{
    void EntityBall::collision(Entity* entity)
    {
        std::string myBallName = getBallName();

        if (entity->getType() != BALL)
            return;

        EntityBall* p_otherBall = (EntityBall*) entity;

        std::string otherBallName = p_otherBall->getBallName();
        LOG_INFO("%s has collided with %s", myBallName.c_str(), otherBallName.c_str());
    }

    std::string EntityBall::getBallName()
    {
        std::string ballName = "Ball " + std::to_string(m_ballID);
        if (m_ballID == 0 && m_subType == CLASSIC)
            ballName = "Cue Ball";
        return ballName;
    }
}