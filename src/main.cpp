#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "window.h"
#include "gl_utils.h"

#include "rendering/shader.h"
#include "rendering/texture.h"
#include "rendering/flexibleMesh.h"

#include "rendering/simpleModel.h"

#include "rendering/model.h"
#include "rendering/lightPoint.h"
#include "rendering/object.h"


#include "camera/camera.h"
// #include "input.h" // included in window
#include "camera/camera_controller_fly.h"
#include "camera/camera_controller_fps.h"
#include "camera/camera_controller_orbit.h"


// matrix operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// funciones

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ((WindowData *)glfwGetWindowUserPointer(window))->input.updateCursor(xpos, ypos);
}

void processInput(GLFWwindow *window, Input* input, float deltaTime)
{
    std::vector<Input::eventKey>* ekeys = input->getEventKeys();
    for (int i = 0; i < ekeys->size(); i++)
    {
        if (glfwGetKey(window, (*ekeys)[i].key) == GLFW_RELEASE && (*ekeys)[i].isPressed)
        {
            input->pressKey((*ekeys)[i].key, deltaTime);
            (*ekeys)[i].isPressed = false;
        }
        else if (glfwGetKey(window, (*ekeys)[i].key) == GLFW_PRESS)
            (*ekeys)[i].isPressed = true;
    }

    std::vector<int>* keys = input->getPollingKeys();
    for (int i = 0; i < keys->size(); i++)
    {
        if (glfwGetKey(window, (*keys)[i]) == GLFW_PRESS)
            input->pressKey((*keys)[i], deltaTime);
    }
}


// shader files
std::string vertexDir = "shaders/default.vert";
std::string fragmentDir = "shaders/default.frag";

// std::string textureDir = "textures/container2.png";


int main()
{

    Window window(1200, 900, "Billar", true);

    if (!window.initWindow()) {
        return 1;
    }

    // enables z-buffer
    glEnable(GL_DEPTH_TEST);


    /////////// LIGHT CUBE ///////////

    float lightCubeVertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    std::vector<AttributeData> lightCubeAttributes;
    lightCubeAttributes.push_back(AttributeData(3, 0));
    Mesh lightCubeMesh;
    lightCubeMesh.setVertex(lightCubeVertices, 36, 3*sizeof(float), lightCubeAttributes);
    lightCubeMesh.create();

    RenderingTemp::SimpleModel lightCube(&lightCubeMesh);
    glm::vec3 lightCubePosition = glm::vec3( 0.7f,  1.2f,  2.0f);
    lightCube.scale(0.2f);
    lightCube.setPosition(&lightCubePosition);
    glm::vec3 lightCubeColor = glm::vec3(1.0f, 1.0f, 1.0f);


    ///// loading shaders /////

    Shader modelShader("shaders/model.vert", "shaders/model.frag");
    if(!modelShader.compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        return 1;
    }

    Shader lightShader("shaders/light.vert", "shaders/light.frag");
    if(!lightShader.compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        return 1;
    }


    /////////// IMGUI init ///////////

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");


    bool drawTriangles = true;

    glm::vec3 controlledPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 controlledDirection = glm::vec3(0.5f, 1.0f, 0.0f);


    /////////// INPUT init ///////////

    glfwSetCursorPosCallback(window.getGLFWwindow(), mouse_callback);

    Input* input = window.getInput();

    input->setKeyAction(MOVE_FORWARDS, GLFW_KEY_W);
    input->setKeyAction(MOVE_LEFT, GLFW_KEY_A);
    input->setKeyAction(MOVE_BACKWARDS, GLFW_KEY_S);
    input->setKeyAction(MOVE_RIGHT, GLFW_KEY_D);

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

    /////////// CAMERA ///////////

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // camera perspective to screen
    glm::mat4 projection = glm::perspective(45.0f, 1200.0f / 800.0f, 0.1f, 100.0f); /// this should be updated when window changes size


    enum CameraType {
        FLY,
        FPS,
        ORBIT
    };
    CameraType currentType = ORBIT;

    CameraController* cameraController = new CameraControllerOrbit(&camera, 2.5f, 3.0f, &controlledPosition);
    bindInputToController(input, cameraController);


    // here goes the models
    Rendering::Model poolTableModel("models/pooltable/Pool table Cavicchi Leonardo 9FT N300818.3ds");
    Rendering::Model ball9Model("models/ball1/Project Name.obj");

    Rendering::Object poolTable(&poolTableModel);
    Rendering::Object ball9(&ball9Model);

    // here goes the lights
    Rendering::LightPoint lightPoint(0);

    unsigned int nFrame = 0;
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while(!window.shouldClose())
    {
        // rendering commands here
        GL(glClearColor(0.1f, 0.1f, 0.15f, 1.0f));
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // ImGui newframe
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!io.WantCaptureMouse)
        {
            // blocks anything here if mouse is over imgui
            // input handling inside
            processInput(window.getGLFWwindow(), input, deltaTime);
        }
        // physics update outside (also update the position of everything)
        cameraController->update();


        glm::mat4 view = camera.getViewMatrix();

        /////////// RENDERING ///////////
        if (drawTriangles)
        {
            lightPoint.setPosition(lightCubePosition);
            lightPoint.updateShader(&modelShader);

            glm::vec3 poolTablePosition = glm::vec3(0.0f);
            poolTable.setPosition(poolTablePosition);
            glm::vec3 poolTableOrientation = glm::vec3(0.0f, 1.0f, 0.0f);
            poolTable.setOrientation(0.0f, poolTableOrientation);
            poolTable.setScaling(0.001f);

            poolTable.draw(&modelShader, view, projection, camera.getPosition());

            ball9.setPosition(controlledPosition);
            ball9.setOrientation(glm::radians(250.0f)*(float)glfwGetTime(), controlledPosition);
            ball9.setScaling(0.001f);

            ball9.draw(&modelShader, view, projection, camera.getPosition());
        }


        /////////// ImGui ///////////

        ImGui::Begin("Debug Window");
        ImGui::Checkbox("Draw Triangles", &drawTriangles);
        ImGui::Separator();
        ImGui::Text("Cube Controls");
        ImGui::SliderFloat3("Position", glm::value_ptr(controlledPosition), -1.5f, 1.5f);
        ImGui::SliderFloat3("Direction", glm::value_ptr(controlledDirection), -1.0f, 1.0f);
        ImGui::Separator();
        ImGui::Text("Light Cube Controls");
        ImGui::SliderFloat3("Light Position", glm::value_ptr(lightCubePosition), -2.0f, 2.0f);
        ImGui::ColorEdit3("Light Color", glm::value_ptr(*lightPoint.getColor()));
        ImGui::Separator();
        ImGui::Text("Camera Controls");
        if (ImGui::Button("Camera fly"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerFly(&camera);
            bindInputToController(input, cameraController);
        }
        if (ImGui::Button("Camera fps"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerFps(&camera);
            bindInputToController(input, cameraController);
        }
        if (ImGui::Button("Camera orbit"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 3.0f, &controlledPosition);
            bindInputToController(input, cameraController);
        }
        ImGui::Text("Frame number %u", nFrame);
        ImGui::Text("FPS: %f", 1/deltaTime);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // check and call events and swap the buffers

        window.update();

        nFrame++;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }


    glfwTerminate();
    return 0;
}