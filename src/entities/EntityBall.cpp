#include "entities/EntityBall.h"
#include "audio.h"


namespace Entities
{
    void EntityBall::collision(Entity* entity, double force)
    {
        std::string myBallName = getBallName();

        if (entity->getType() != BALL)
            return;

        EntityBall* p_otherBall = (EntityBall*) entity;

        std::string otherBallName = p_otherBall->getBallName();
        // LOG_INFO("%s has collided with %s, with a force of %f", myBallName.c_str(), otherBallName.c_str(), force);

        irrklang::ISound* sound = Audio::AUDIO_FUNCTIONS.play2D("media/collision.wav", false, true, true);
        sound->setVolume(force/MAX_BALL_FORCE);
        sound->setIsPaused(false);
    }

    std::string EntityBall::getBallName()
    {
        std::string ballName = "Ball " + std::to_string(m_ballID);
        if (m_ballID == 0 && m_subType == CLASSIC)
            ballName = "Cue Ball";
        return ballName;
    }
}