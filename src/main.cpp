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


// funciones

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Input* input = &((WindowData *)glfwGetWindowUserPointer(window))->input;
    input->updateCursor(xpos, ypos);
}

void processInput(GLFWwindow *window, Input* input, float deltaTime, CameraController* controller)
{

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

// std::string textureDir = "textures/container2.png";


int main()
{

    Window window(1200, 900, "Billar", true);

    if (!window.initWindow()) {
        return 1;
    }

    // enables z-buffer
    glEnable(GL_DEPTH_TEST);

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
    glm::vec3 lightCubePosition = glm::vec3( 0.7f,  0.2f,  2.0f);
    lightCube.scale(0.2f);
    lightCube.setPosition(&lightCubePosition);
    glm::vec3 lightCubeColor = glm::vec3(1.0f, 1.0f, 1.0f);




    float cubeVertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };




    std::vector<AttributeData> cubeAttributes;
    cubeAttributes.push_back(AttributeData(3, 0));
    cubeAttributes.push_back(AttributeData(3, (void*)(3*sizeof(float))));
    cubeAttributes.push_back(AttributeData(2, (void*)(6*sizeof(float))));

    Mesh cubeMesh;
    cubeMesh.setVertex(cubeVertices, 36, 8*sizeof(float), cubeAttributes);
    cubeMesh.create();

    Texture diffuseTexture("textures/container2.png");
    if (!diffuseTexture.create())
    {
        LOG_ERROR("Failed creating the texture");
        return 1;
    }
    Texture specularTexture("textures/container2_specular.png");
    if (!specularTexture.create())
    {
        LOG_ERROR("Failed creating the texture");
        return 1;
    }
    Texture emissionTexture("textures/matrix.jpg");
    if (!emissionTexture.create())
    {
        LOG_ERROR("Failed creating the texture");
        return 1;
    }

    RenderingTemp::SimpleModel cube(&cubeMesh);



    Shader basicShader(vertexDir, fragmentDir);
    if(!basicShader.compileShaders())
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

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // camera perspective to screen
    glm::mat4 projection = glm::perspective(45.0f, 1200.0f / 800.0f, 0.1f, 100.0f); /// this should be updated when window changes size



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");


    bool drawTriangles = true;
    glm::vec3 cubePosition = glm::vec3(0, 0, 0);
    glm::vec3 cubeDirection = glm::vec3(0.5f, 1.0f, 0.0f);

    cube.setPosition(&cubePosition);


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
        GL(glClearColor(0.1f, 0.1f, 0.15f, 1.0f));
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // ImGui newframe
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // if (!io.WantCaptureMouse)
        // {
        //     // blocks anything here if mouse is over imgui
        //     // input handling inside
        // }
        //     // physics update outside (also update the position of everything)
        processInput(window.getGLFWwindow(), input, deltaTime, cameraController);   // this should be changed later

        // // model transformations
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, cubePosition);
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), cubeDirection);

        glm::mat4 view = camera.getViewMatrix();

        // float timeValue = glfwGetTime();
        // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(basicShader.getProgram(), "ourColor");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        glm::vec3 cubePositions[] = {
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

        glm::vec3 pointLightPositions[] = {
            lightCubePosition,
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
        };

        if (drawTriangles)
        {
            // texture.activate();
            basicShader.activate();   // need to activate before setting uniforms
            // basicShader.setUniformVec3("lightColor", lightCubeColor);
            // basicShader.setUniformVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
            // basicShader.setUniformVec3("lightPos", lightCubePosition);
            basicShader.setUniformVec3("viewPos", camera.getPosition());

            basicShader.setUniformVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
            // basicShader.setUniformVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
            basicShader.setUniformInt("material.diffuse", 0);
            // basicShader.setUniformVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
            basicShader.setUniformInt("material.specular", 1);
            basicShader.setUniformInt("material.emission", 2);
            basicShader.setUniformFloat("material.shininess", 32.0f);

            // point light loop
            for (int i = 0; i < 4; i++)
            {
                basicShader.setUniformVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);

                basicShader.setUniformFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
                basicShader.setUniformFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
                basicShader.setUniformFloat("pointLights[" + std::to_string(i) + "].cuadratic", 0.0032f);
                

                basicShader.setUniformVec3("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(0.1f));
                basicShader.setUniformVec3("pointLights[" + std::to_string(i) + "].diffuse", lightCubeColor * 0.5f);
                basicShader.setUniformVec3("pointLights[" + std::to_string(i) + "].specular", lightCubeColor * 1.0f);

            }
            // directional light (the sun)
            basicShader.setUniformVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));

            basicShader.setUniformVec3("dirLight.ambient", glm::vec3(0.1f));
            basicShader.setUniformVec3("dirLight.diffuse", lightCubeColor * 0.5f);
            basicShader.setUniformVec3("dirLight.specular", lightCubeColor * 1.0f);

            // spot light (flashlight)
            basicShader.setUniformVec3("spotLight.position", camera.getPosition());
            basicShader.setUniformVec3("spotLight.direction", camera.getFront());
            basicShader.setUniformFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            basicShader.setUniformFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

            basicShader.setUniformFloat("spotLight.constant", 1.0f);
            basicShader.setUniformFloat("spotLight.linear", 0.09f);
            basicShader.setUniformFloat("spotLight.quadratic", 0.0032f);

            basicShader.setUniformVec3("spotLight.ambient", glm::vec3(0.1f));
            basicShader.setUniformVec3("spotLight.diffuse", lightCubeColor *1.0f);
            basicShader.setUniformVec3("spotLight.specular", lightCubeColor * 1.0f);

            cube.setPosition(&cubePosition);
            cube.setOrientation(glm::radians(25.0f)*(float)glfwGetTime(), cubeDirection);
            // cube.rotate(glm::radians(10.0f)*deltaTime, cubeDirection);

            diffuseTexture.activate(0);
            specularTexture.activate(1);
            emissionTexture.activate(2);

            cube.draw(&basicShader, view, projection);

            for (int i = 0; i < 9; i++)
            {
                cube.setPosition(&cubePositions[i]);
                cube.setOrientation(glm::radians(20.0f*i), glm::vec3(1.0f, 0.3f, 0.5f));

                cube.redraw(&basicShader);  // cube was previously drawn so we can redraw here
            }

            lightShader.activate();
            lightShader.setUniformVec3("lightColor", lightCubeColor);

            lightCube.setPosition(&pointLightPositions[0]);
            lightCube.draw(&lightShader, view, projection);

            for (int i = 1; i < 4; i++)
            {
                lightCube.setPosition(&pointLightPositions[i]);
                lightCube.redraw(&lightShader);
            }

        }



        ImGui::Begin("My name is window, ImGUI window");
        ImGui::Text("Cube Controls");
        ImGui::Checkbox("Draw Triangles", &drawTriangles);
        ImGui::SliderFloat3("Position", glm::value_ptr(cubePosition), -1.5f, 1.5f);
        ImGui::SliderFloat3("Direction", glm::value_ptr(cubeDirection), -1.0f, 1.0f);
        ImGui::SliderFloat3("Light Position", glm::value_ptr(lightCubePosition), -2.0f, 2.0f);
        ImGui::ColorEdit3("Light Color", glm::value_ptr(lightCubeColor));
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
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 3.0f, &cubePosition);
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