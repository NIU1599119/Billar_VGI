#include "menu/menu.h"
#include "menu/resource_manager.h"
#include "menu/shader.h"
#include "menu/texture.h"
#include "menu/rendering/SpriteRenderer.h"

#include "window.h"
#include "gl_utils.h"
#include <iostream>

// Matrix
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Menu
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

SpriteRenderer* Renderer;

int initMenu(int& opcio, Window& window, float& Width, float& Height)
{
    // Creaci� de ventana
    window = Window(Width, Height, "Billar", true, true);

    if (!window.initWindow()) {
        return -1;
    }

    Input* input = window.getInput();

    // Creaci� de la detecci� d'inputs

    bool clicked = false;

    input->setKeyAction(EXIT, GLFW_KEY_BACKSPACE, false);
    input->setKeyAction(EXIT, GLFW_KEY_ESCAPE, false);
    input->setActionFunction(EXIT, [&window](float delaTime) { window.close(); });

    //////// this is for 3d interaction, don't use SWITCH_MOUSE on menu
    // input->setKeyAction(SWITCH_MOUSE, GLFW_KEY_M, false);
    // input->setActionFunction(SWITCH_MOUSE, [&window, input](float deltaTime) {
    //     if (input->mouseIsCaptured())
    //     {
    //         glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //         input->uncaptureMouse();
    //     }
    //     else
    //     {
    //         glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);    // hides cursor and sets it to middle
    //         input->captureMouse();
    //     }
    // });

    // menu cursor
    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);      // let cursor free
    input->captureMouse();                                                          // but capture the inputs (position)

    glm::vec2 mousePosition = glm::vec2(0.0f);

    // mouse input setup
    input->setMouseCallback([&mousePosition](float posX, float posY) {
        // LOG_DEBUG("Mouse is now at (%f, %f)", posX, posY);
        mousePosition = glm::vec2(posX, posY);
    }, false);  // false indica posicion absoluta para obtener la posicion en pixeles del cursor (en lugar de la posicion relativa que tiene que ver con el anterior frame)

    input->setKeyAction(LEFT_CLICK, GLFW_MOUSE_BUTTON_LEFT, false);
    input->setActionFunction(LEFT_CLICK, [&mousePosition, &clicked](float deltaTime) {
        LOG_DEBUG("Mouse clicked at (%f, %f)", mousePosition.x, mousePosition.y);
        clicked = true;
    });


    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    // Creaci� dels botons
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width),
        static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("textures/fondo3.png", true, "menu");
    ResourceManager::LoadTexture("textures/menu/2.png", true, "button1");
    ResourceManager::LoadTexture("textures/menu/3.png", true, "button2");
    ResourceManager::LoadTexture("textures/menu/4.png", true, "button3");
    ResourceManager::LoadTexture("textures/menu/5_sinfondo.png", true, "button4");
    ResourceManager::LoadTexture("textures/menu/6_sinfondo.png", true, "button5");

    // Creacio del bucle de la ventana

    while (!window.shouldClose())
    {
        // Agafa els inputs

        clicked = false;

        window.processInput(deltaTime);
      
        Renderer->DrawSprite(ResourceManager::GetTexture("menu"),
            glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        opcio = 0;
        std::cout << mousePosition.x << ", " << mousePosition.y << std::endl;
        if ((mousePosition.x > 180.0f && mousePosition.y > 400.0f) && (mousePosition.x < 575.0f && mousePosition.y < 730.0f))
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button1"),
                glm::vec2(275.0f, 550.0f), glm::vec2(600, 450), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

            opcio = 1;
        }
        else
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button1"),
                glm::vec2(275.0f, 550.0f), glm::vec2(600, 450), 0.0f, glm::vec3(0.7f, 0.7f, 0.7f));
        }
        
        if ((mousePosition.x > 650.0f && mousePosition.y > 400.0f) && (mousePosition.x < 1040.0f && mousePosition.y < 730.0f))
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button2"),
                glm::vec2(990.0f, 550.0f), glm::vec2(600, 450), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            opcio = 2;
        }
        else
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button2"),
                glm::vec2(990.0f, 550.0f), glm::vec2(600, 450), 0.0f, glm::vec3(0.7f, 0.7f, 0.7f));
        }

        if ((mousePosition.x > 1120.0f && mousePosition.y > 400.0f) && (mousePosition.x < 1510.0f && mousePosition.y < 730.0f))
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button3"),
                glm::vec2(1700.0f, 550.0f), glm::vec2(600, 450), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            opcio = 3;
        }
        else
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button3"),
                glm::vec2(1700.0f, 550.0f), glm::vec2(600, 450), 0.0f, glm::vec3(0.7f, 0.7f, 0.7f));
        }

        if ((mousePosition.x > 430.0f && mousePosition.y > 850.0f) && (mousePosition.x < 615.0f && mousePosition.y < 922.0f))
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button4"),
                glm::vec2(650.0f, 1150.0f), glm::vec2(300, 125), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            opcio = 4;
        }
        else
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button4"),
                glm::vec2(650.0f, 1150.0f), glm::vec2(300, 125), 0.0f, glm::vec3(0.7f, 0.7f, 0.7f));
        }

        if ((mousePosition.x > 1025.0f && mousePosition.y > 850.0f) && (mousePosition.x < 1205.0f && mousePosition.y < 922.0f))
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button5"),
                glm::vec2(1550.0f, 1150.0f), glm::vec2(300, 125), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            opcio = 4;
        }
        else
        {
            Renderer->DrawSprite(ResourceManager::GetTexture("button5"),
                glm::vec2(1550.0f, 1150.0f), glm::vec2(300, 125), 0.0f, glm::vec3(0.7f, 0.7f, 0.7f));
        }

        // Actualitza el frame
        window.update();

        if (opcio != 0 && clicked)
        {
            if (opcio == 1)
                return opcio;
            else
                window.close();
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    return opcio;
}

int initResolution(float& x, float& y)
{
    Window window(700, 500, "Billar", true, false);

    if (!window.initWindow()) {
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!window.shouldClose())
    {
        // rendering commands here
        GL(glClearColor(0.1f, 0.1f, 0.15f, 1.0f));
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // ImGui newframe
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug Window");
        ImGui::Text("Selecciona resolució");

        if (ImGui::Button("720p"))
        {
            window.close();
            x = 1280;
            y = 720;
        }

        if (ImGui::Button("1080p"))
        {
            window.close();
            x = 1920;
            y = 1080;
        }

        if (ImGui::Button("1440p"))
        {
            window.close();
            x = 2560;
            y = 1440;
        }

        if (ImGui::Button("2160p"))
        {
            window.close();
            x = 3840;
            y = 2160;
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // check and call events and swap the buffers

        window.update();
    }

    glfwTerminate();

    return 0;
}