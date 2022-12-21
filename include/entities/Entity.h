#pragma once
#include <glm/glm.hpp>
#include "debug.h"

#include "game/gamemodes.h"
#include "game/BaseGameState.h"

namespace Entities 
{
    enum TYPE {
        NOTHING = 0,
        TABLE = 1,
        BALL = 2
    };

    class Entity
    {
    public:
        Entity(TYPE type, BaseGameState* gamestate) : m_type(type), m_gamestate(gamestate), m_subType(gamestate->getGamemode()) {};

        // void setPosition();
        // void getPosition();

        TYPE getType() { return m_type; };
        GAMEMODE getSubType() { return m_subType; };

        virtual void collision(Entity* entity, double force) = 0;

        bool getIsDisabled() { return m_disabled; };

        void disable() { m_disabled = true; };
        void enable() { m_disabled = false; };

    protected:
        // glm::vec3 m_position;
        TYPE m_type;
        GAMEMODE m_subType;

        BaseGameState* m_gamestate;

        bool m_disabled = false;
    };

};
