#pragma once
#include "entities/Entity.h"

namespace Entities
{
    enum TABLE_PART {
        CLOTH,
        RAIL
    };

    class EntityTable : public Entity
    {
    public:
        EntityTable(TABLE_PART part, BaseGameState* gamestate) : Entity(TABLE, gamestate), m_part(part) {};

        void collision(Entity* entity, double force) {};

    private:
        TABLE_PART m_part;
    };
};
