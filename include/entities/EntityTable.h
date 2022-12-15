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
        EntityTable(GAME_TYPE gameType, TABLE_PART part) : Entity(TABLE, gameType), m_part(part) {};
    private:
        TABLE_PART m_part;
    };
};
