#include "menu/menu.h"
#include "menu/resource_manager.h"
#include "menu/shader.h"
#include "menu/texture.h"
#include "menu/rendering/SpriteRenderer.h"

#include "window.h"
#include <iostream>

// Matrix
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SpriteRenderer* Renderer;

int initMenu(int& opcio)
{
    // Creació de ventana 
    float Width = 1920;
    float Height = 1080;
    Window window(Width, Height, "Billar", true, true);

    if (!window.initWindow()) {
        return -1;
    }

    Input* input = window.getInput();

    // Creació de la detecció d'inputs

    input->setKeyAction(EXIT, GLFW_KEY_BACKSPACE, false);
    input->setKeyAction(EXIT, GLFW_KEY_ESCAPE, false);
    input->setActionFunction(EXIT, [&window](float delaTime) { window.close(); });

    input->setKeyAction(SWITCH_MOUSE, GLFW_KEY_M, false);
    input->setActionFunction(SWITCH_MOUSE, [&window, input](float deltaTime) {
        if (input->mouseIsCaptured())
        {
            glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            input->uncaptureMouse();
        }
        else
        {
            glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);    // hides cursor and sets it to middle
            input->captureMouse();
        }
    });

    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    // Creació dels botons
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
    ResourceManager::LoadTexture("textures/minecraftmenu.png", true, "menu");
    ResourceManager::LoadTexture("textures/container2.png", true, "menu2");

    // Creacio del bucle de la ventana

    while (!window.shouldClose())
    {
        // Agafa els inputs
        processInput(window.getGLFWwindow(), input, deltaTime);
      
        Renderer->DrawSprite(ResourceManager::GetTexture("menu"),
            glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        Renderer->DrawSprite(ResourceManager::GetTexture("menu2"),
            glm::vec2(100.0f, 100.0f), glm::vec2(300, 200), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        // Actualitza el frame
        window.update();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

}