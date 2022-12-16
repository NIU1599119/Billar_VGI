// menu
#include "menu/menu.h"

//game
#include "game.h"

//window
#include "window.h"

// std::string textureDir = "textures/container2.png";
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "audio.h"

int main()
{
    Window window;
    window = Window(1280, 720, "Billar", true, false);

    if (!window.initWindow()) {
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");


    float x, y;
    initResolution(x, y, window);

    /* GENIOS
    if (Audio::initAudio() != 0)
    {
        return 1; // error inicializando audio
    }
    Audio::engine->setSoundVolume(0.1f);
    Audio::engine->play2D("media/blues.ogg", true);*/
    Audio::AUDIO_FUNCTIONS.setVolume(1.0);

    irrklang::ISound* music = Audio::AUDIO_FUNCTIONS.play2D("media/blues.ogg", true, true, true);
    music->setVolume(0.6);


    bool continuarJugando = true;
    while(continuarJugando) {
        int opcio = 0;
        music->setIsPaused(false);
        initMenu(opcio, window, x, y);

        if (opcio == 1)
        {
            music->setIsPaused(true);
            continuarJugando = (Game(window) == 0);
            if (!continuarJugando)
            {
                LOG_ERROR("El juego ha crasheado o no ha podido cargar");
                return 1;
            }
        }
        else
            continuarJugando = false;
    }

    music->drop();

    window.close();

    return 0;
}
