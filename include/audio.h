#pragma once

#include "debug.h"
#include <iostream>
#include <../irrKlang/include/irrKlang.h>


/* GENIOS
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
*/


class Audio
{
    public:
        Audio() : engine(nullptr) {
            engine = irrklang::createIrrKlangDevice();
            if (!engine) {
                std::cout << "ERROR CREATING SOUND ENGINE" << std::endl;
            }
        }
        ~Audio() {
            engine->drop();
        }

        // Play sounds
        irrklang::ISound* play2D(const char* path, bool looped, bool startPaused, bool track = false)
        {
            return  engine->play2D(path, looped, startPaused, track);
        }

        // volumen desde 0.0 a 1.0
        void setVolume(float volume)
        {
            engine->setSoundVolume(volume);
        }

        static Audio AUDIO_FUNCTIONS;
    private:
        irrklang::ISoundEngine* engine;
};
