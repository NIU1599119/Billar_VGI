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
        EntityTable(GAMEMODE gameType, TABLE_PART part) : Entity(TABLE, gameType), m_part(part) {};

        void collision(Entity* entity) {};
        void collision(Entity* entity, double force) {};

    private:
        TABLE_PART m_part;
    };
};
