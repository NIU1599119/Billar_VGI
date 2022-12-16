#pragma once
#include <glm/glm.hpp>
#include "debug.h"


namespace Entities 
{
    enum TYPE {
        TABLE,
        BALL
    };

    enum GAME_TYPE {
        CLASSIC,
        CARAMBOLA,
        FREE_SHOTS
    };

    class Entity
    {
    public:
        Entity(TYPE type, GAME_TYPE subType) : m_type(type), m_subType(subType) {};

        // void setPosition();
        // void getPosition();

        TYPE getType() { return m_type; };
        GAME_TYPE getSubType() { return m_subType; };

        virtual void collision(Entity* entity) = 0;
        virtual void collision(Entity* entity, double force) = 0;

    protected:
        // glm::vec3 m_position;
        TYPE m_type;
        GAME_TYPE m_subType;
    };

};
