#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "window.h"
#include "gl_utils.h"

#include "rendering/shaderProgram.h"
#include "rendering/texture.h"
#include "rendering/mesh.h"


#include "camera/camera.h"
// #include "input.h" // included in window
#include "camera/camera_controller_fly.h"
#include "camera/camera_controller_fps.h"
#include "camera/camera_controller_orbit.h"


// matrix operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

bool mouseCapture = false;
bool pressing = true;

// Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));



// funciones

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Input* input = &((WindowData *)glfwGetWindowUserPointer(window))->input;
    input->updateCursor(xpos, ypos);
    // camera.offsetAngles(xoffset, yoffset);
}

// void empty_callback(GLFWwindow* window, double xpos, double ypos) {};

void processInput(GLFWwindow *window, Input* input, float deltaTime, CameraController* controller)
{
    // const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    // glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //     offset += cameraSpeed * camera.getFront();
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //     offset -= cameraSpeed * camera.getFront();
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //     offset -= cameraSpeed * camera.getRight();
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //     offset += cameraSpeed * camera.getRight();
    // camera.offsetPosition(offset);

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        pressing = true;
    }
    else
    {
        if (pressing)
        {
            if (mouseCapture)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                input->lockMouse();
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                input->unlockMouse();
            }
            mouseCapture = !mouseCapture;
        }
        pressing = false;
    }

    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    std::vector<int> keys = input->getPollingKeys();
    for (int i = 0; i < keys.size(); i++)
    {
        if (glfwGetKey(window, keys[i]) == GLFW_PRESS)
            input->pressKey(keys[i], deltaTime);
    }
    controller->update();
}


// shader files
std::string vertexDir = "shaders/default.vert";
std::string fragmentDir = "shaders/default.frag";

std::string textureDir = "textures/minecraft-grass.png";


int main()
{

    Window window(1200, 900, "Billar");

    if (!window.initWindow()) {
        return 1;
    }

    float uDiv = 1.0f/4.0f;
    float vDiv = 1.0f/3.0f;

    float cubeVertices[] {
        -0.5f, -0.5f, -0.5f,  0.0f, vDiv,
        0.5f, -0.5f, -0.5f,  uDiv, vDiv,
        0.5f,  0.5f, -0.5f,  uDiv, 0,
        0.5f,  0.5f, -0.5f,  uDiv, 0,
        -0.5f,  0.5f, -0.5f,  0.0f, 0,
        -0.5f, -0.5f, -0.5f,  0.0f, vDiv,

        -0.5f, -0.5f,  0.5f,  0.0f, 2*vDiv,
        0.5f, -0.5f,  0.5f,  uDiv, 2*vDiv,
        0.5f,  0.5f,  0.5f,  uDiv, 3*vDiv,
        0.5f,  0.5f,  0.5f,  uDiv, 3*vDiv,
        -0.5f,  0.5f,  0.5f,  0.0f, 3*vDiv,
        -0.5f, -0.5f,  0.5f,  0.0f, 2*vDiv,

        -0.5f,  0.5f,  0.5f,  3*uDiv, 2*vDiv,
        -0.5f,  0.5f, -0.5f,  3*uDiv, vDiv,
        -0.5f, -0.5f, -0.5f,  4*uDiv, vDiv,
        -0.5f, -0.5f, -0.5f,  4*uDiv, vDiv,
        -0.5f, -0.5f,  0.5f,  4*uDiv, 2*vDiv,
        -0.5f,  0.5f,  0.5f,  3*uDiv, 2*vDiv,

        0.5f,  0.5f,  0.5f,  2*uDiv, 2*vDiv,
        0.5f,  0.5f, -0.5f,  2*uDiv, vDiv,
        0.5f, -0.5f, -0.5f,  uDiv, vDiv,
        0.5f, -0.5f, -0.5f,  uDiv, vDiv,
        0.5f, -0.5f,  0.5f,  uDiv, 2*vDiv,
        0.5f,  0.5f,  0.5f,  2*uDiv, 2*vDiv,

        -0.5f, -0.5f, -0.5f,  0.0f, vDiv,
        0.5f, -0.5f, -0.5f,  uDiv, vDiv,
        0.5f, -0.5f,  0.5f,  uDiv, vDiv*2,
        0.5f, -0.5f,  0.5f,  uDiv, vDiv*2,
        -0.5f, -0.5f,  0.5f,  0.0f, vDiv*2,
        -0.5f, -0.5f, -0.5f,  0.0f, vDiv,

        -0.5f,  0.5f, -0.5f,  3*uDiv, vDiv,
        0.5f,  0.5f, -0.5f,  2*uDiv, vDiv,
        0.5f,  0.5f,  0.5f,  2*uDiv, vDiv*2,
        0.5f,  0.5f,  0.5f,  2*uDiv, vDiv*2,
        -0.5f,  0.5f,  0.5f,  3*uDiv, vDiv*2,
        -0.5f,  0.5f, -0.5f,  3*uDiv, vDiv
    };

    std::vector<AttributeData> cubeAttributes;
    cubeAttributes.push_back(AttributeData(3, 0));
    cubeAttributes.push_back(AttributeData(2, (void*)(3*sizeof(float))));


    Mesh cubeMesh;

    cubeMesh.setVertex(cubeVertices, 36, 5*sizeof(float), cubeAttributes);

    cubeMesh.create();


    // enables z-buffer
    glEnable(GL_DEPTH_TEST);


    ShaderProgram shaderProgram(vertexDir, fragmentDir);

    if(!shaderProgram.compileShaders())
    {
        std::cout << "Failed compiling shader\n";
        return 1;
    }

    Texture texture(textureDir);

    if (!texture.createTexture())
    {
        std::cout << "Failed creating the texture\n";
        return 1;
    }


    // model transform
    glm::mat4 model = glm::mat4(1.0f);



    // camera code

    // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);          // translate camera 3 z-units       (camera is far away from objects)

    // glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    // glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // float radius = 10.0f;

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


    // camera perspective to screen
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(45.0f, 1200.0f / 800.0f, 0.1f, 100.0f);



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");


    bool drawTriangles = true;
    glm::vec3 cubePosition = glm::vec3(0, 0, 0);
    glm::vec3 cubeDirection = glm::vec3(0.5f, 1.0f, 0.0f);


    // input initialization
    glfwSetCursorPosCallback(window.getGLFWwindow(), mouse_callback);

    Input* input = window.getInput();

    input->setKeyAction(MOVE_FORWARDS, GLFW_KEY_W);
    input->setKeyAction(MOVE_LEFT, GLFW_KEY_A);
    input->setKeyAction(MOVE_BACKWARDS, GLFW_KEY_S);
    input->setKeyAction(MOVE_RIGHT, GLFW_KEY_D);

    enum CameraType {
        FLY,
        FPS,
        ORBIT
    };
    CameraType currentType = ORBIT;

    CameraController* cameraController = new CameraControllerOrbit(&camera, 2.5f, 3.0f, &cubePosition);
    bindInputToController(input, cameraController);


    unsigned int nFrame = 0;
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while(!window.shouldClose())
    {
        // rendering commands here
        glClearColor(0.3f, 0.3f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui newframe
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // if (!io.WantCaptureMouse)
        // {
        //     // input handling here
        // }
        processInput(window.getGLFWwindow(), input, deltaTime, cameraController);


        // activate shader
        // glUseProgram(shaderProgram);
        shaderProgram.activate();


        // model transformations
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePosition);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), cubeDirection);


        glm::mat4 view = camera.getViewMatrix();


        unsigned int modelLoc = glGetUniformLocation(shaderProgram.getProgram(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        unsigned int viewLoc = glGetUniformLocation(shaderProgram.getProgram(), "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int projectionLoc = glGetUniformLocation(shaderProgram.getProgram(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        // float timeValue = glfwGetTime();
        // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(shaderProgram.getProgram(), "ourColor");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };



        if (drawTriangles)
        {
            texture.activateTexture();

            cubeMesh.draw();

            for (int i = 0; i < 10; i++)
            {
                glm::mat4 iModel = glm::mat4(1.0f);
                iModel = glm::translate(iModel, cubePositions[i]);
                float angle = 20.0f*i;
                iModel = glm::rotate(iModel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(iModel));

                cubeMesh.redraw();  // cube was previously drawn so we can redraw here
            }

        }



        ImGui::Begin("My name is window, ImGUI window");
        ImGui::Text("Cube Controls");
        ImGui::Checkbox("Draw Triangles", &drawTriangles);
        ImGui::SliderFloat3("Position", glm::value_ptr(cubePosition), -10.0f, 3.0f);
        ImGui::SliderFloat3("Direction", glm::value_ptr(cubeDirection), -1.0f, 1.0f);
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
        if (ImGui::Button("Camera orbit"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 3.0f, &cubePosition);
            bindInputToController(input, cameraController);
        }
        ImGui::Text("Frame number %u", nFrame);
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