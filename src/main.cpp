#pragma once

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
    window = Window(1280, 720, "Billar", true, true);

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

    Audio::engine->play2D("media/blues.ogg", true);*/

    Audio::AUDIO_FUNCTIONS.play2D("media/blues.ogg", true, false);

    int opcio = 0;
    initMenu(opcio, window, x, y);

    if (opcio == 1)
        return Game(window);
    else
        return 0;

    //Audio::deleteAudio(); GENIOS

    return 0;
}
