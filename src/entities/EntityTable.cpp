#include "audio.h"
#include "entities/EntityTable.h"
#include "entities/EntityBall.h"	// for the MAX_BALL_FORCE

namespace Entities
{
	void EntityTable::collision(Entity* entity, double force)
	{
		if (m_subType != RAIL)
			return;

		irrklang::ISound* sound = Audio::AUDIO_FUNCTIONS.play2D("media/colision pared.wav", false, true, true);
		sound->setVolume(force / MAX_BALL_FORCE);
		sound->setIsPaused(false);
	}
}