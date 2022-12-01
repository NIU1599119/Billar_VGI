#pragma once

#include "debug.h"
#include <irrKlang.h>

namespace Audio
{

    irrklang::ISoundEngine* engine;

    int initAudio()
    {
        engine = irrklang::createIrrKlangDevice();
        if (!engine)
            return 1; // error starting up the engine

        return 0;
    }

    void deleteAudio()
    {
        engine->drop();
    }
}
