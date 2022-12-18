#pragma once
#include <glm/glm.hpp>
#include "debug.h"

#include "game/gamemodes.h"

namespace Entities 
{
    enum TYPE {
        TABLE,
        BALL
    };

    class Entity
    {
    public:
        Entity(TYPE type, GAMEMODE subType) : m_type(type), m_subType(subType) {};

        // void setPosition();
        // void getPosition();

        TYPE getType() { return m_type; };
        GAMEMODE getSubType() { return m_subType; };

        virtual void collision(Entity* entity) = 0;
        virtual void collision(Entity* entity, double force) = 0;

    protected:
        // glm::vec3 m_position;
        TYPE m_type;
        GAMEMODE m_subType;
    };

};
