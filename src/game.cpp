#include "game.h"

int Game(Window& window) {

    // enables z-buffer
    glEnable(GL_DEPTH_TEST);

    ///// loading shaders /////

    Rendering::Shader modelShader("shaders/model.vert", "shaders/model.frag");
    if(!modelShader.compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        return 1;
    }

    Rendering::Shader lightShader("shaders/light.vert", "shaders/light.frag");
    if(!lightShader.compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        return 1;
    }

    #ifdef DEBUG_SHADER
    Rendering::Shader debugShader("shaders/debug.vert", "shaders/debug.frag");
    if(!debugShader.compileShaders())
    {
        LOG_ERROR("Failed compiling debugShader");
        return 1;
    }
    #else
    Rendering::Shader debugShader("shaders/debug.vert", "shaders/debug.frag");
    if(!debugShader.compileShaders())
    {
        LOG_ERROR("Failed compiling debugShader");
        return 1;
    }
    #endif

    ///// rendering /////
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    Rendering::RenderEngine3D renderEngine(&camera, modelShader, &lightShader, &debugShader);
    //int poolRenderID = renderEngine.createObject(std::string("models/pool_table/scene.gltf"), 0.1245);
    //renderEngine.updateObject(poolRenderID, glm::vec3(0.0), glm::quat(1.0, 0.0, 0.0, 0.0));
    int blackBallRenderID = renderEngine.createObject(std::string("models/PoolBall/Pool.obj"), 0.05715 / 2);
    renderEngine.updateObject(blackBallRenderID, glm::vec3(0.0, 0.8, 0.1), glm::quat(1.0, 0.0, 0.0, 0.0));
    std::vector<int> ballsRenderIDs;
    ballsRenderIDs.push_back(blackBallRenderID);

    Rendering::Model ballModel("models/PoolBall1/Pool.obj");
    for (int i = 0; i < 15; i++) // create 15 balls
    {
        int idx = renderEngine.createObject(&ballModel, 0.05715 / 2);
        ballsRenderIDs.push_back(idx);
        renderEngine.updateObject(idx, glm::vec3(0.0, 0.8, 0.1), glm::quat(1.0, 0.0, 0.0, 0.0));
    }

    std::vector<int> lightsRenderID;
    for (int i = 0; i < 5; i++)
    {
        int idx = renderEngine.addLight();
        lightsRenderID.push_back(idx);
        renderEngine.setLightPosition(idx, glm::vec3(-1.0 + (i / 2.0), 2.5, 0.0));
        renderEngine.setLightColor(idx, glm::vec3(1.0, 1.0, 1.0));
        renderEngine.setLightPolinomial(idx, 1.0, 0.09, 0.0032);
    }

    /////////// IMGUI init ///////////

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool drawTriangles = true;

    //glm::vec3 controlledPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    //glm::vec3 controlledDirection = glm::vec3(0.5f, 1.0f, 0.0f);

    /////////// INPUT init ///////////

    Input* input = window.getInput();

    input->setKeyAction(MOVE_FORWARDS, GLFW_KEY_W);
    input->setKeyAction(MOVE_LEFT, GLFW_KEY_A);
    input->setKeyAction(MOVE_BACKWARDS, GLFW_KEY_S);
    input->setKeyAction(MOVE_RIGHT, GLFW_KEY_D);

    bool shouldExit = false;
    //input->setKeyAction(EXIT, GLFW_KEY_BACKSPACE, false);
    input->setKeyAction(EXIT, GLFW_KEY_ESCAPE, false);
    input->setActionFunction(EXIT, [&shouldExit](float delaTime) { LOG_DEBUG("Exiting Game");shouldExit = true; });

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

    //Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // camera perspective to screen
    //glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f); /// this should be updated when window changes size


    enum CameraType {
        FLY,
        FPS,
        ORBIT
    };
    CameraType currentType = ORBIT;


    glm::vec3 cameraOrbitPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    CameraController* cameraController = new CameraControllerOrbit(&camera, 2.5f, 1.0f, &cameraOrbitPosition);
    bindInputToController(input, cameraController);

    //////////// FISICAS ////////////
    ///-----initialization_start-----

    EntityControllerSystem ECS(CLASSIC, input, &camera, &renderEngine, ballsRenderIDs);


    ///// INPUTS FOR BALL //////

    // btDiscreteDynamicsWorld* dynamicsWorld = ECS.getDynamicsWorld();
    // int ballsIndex = ECS.getBallsIndex();


    unsigned int nFrame = 0;
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    Rendering::RuntimeModelEditor runtimeModelEditor(&renderEngine);

    while (!window.shouldClose() && !shouldExit)
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
            window.processInput(deltaTime);
        }
        // physics update outside (also update the position of everything)
        cameraController->update();

        // ECS.StepSimulation(deltaTime, 100, 0.001f);
        ECS.update(deltaTime, &cameraOrbitPosition);
        // dynamicsWorld->stepSimulation(deltaTime, 100, 0.001f);

        //glm::mat4 view = camera.getViewMatrix();

        /////////// RENDERING ///////////
        if (drawTriangles)
        {
            //// update rendering data
            // ECS.draw(&modelShader, &camera, projection, controlledPosition, &debugShader);

            // for (int i = 0; i < 16; i++)
            // {
            //     btCollisionObject* obj = ballsPhisics[i];
            //     btRigidBody* body = btRigidBody::upcast(obj);
            //     btTransform trans;
            //     if (body && body->getMotionState())
            //         body->getMotionState()->getWorldTransform(trans);
            //     else
            //         trans = obj->getWorldTransform();
            //     glm::vec3 pos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
            //     glm::quat orient = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());

            //     if (i == 0)
            //         cameraOrbitPosition = pos;

            //     renderEngine.updateObject(ballsRenderIDs[i], pos, orient);
            // }

            //// rendering update

            renderEngine.updateShaderView();
            renderEngine.drawAll();

            renderEngine.drawLights();

            #ifdef DEBUG_SHADER
            std::vector<Rendering::CollisionBox>* p_rigidObjects = ECS.getCollisionsBoxPtr();
            for (int i = 0; i < (*p_rigidObjects).size(); i++)
            {
                //rigidObjects[i].draw(&debugShader, view, projection);
                (*p_rigidObjects)[i].draw(renderEngine.getDebuggingShader());
            }
            #endif
        }


        /////////// ImGui ///////////

        ImGui::Begin("Debug Window");
        ImGui::Checkbox("Draw Triangles", &drawTriangles);
        ImGui::Separator();
        // ImGui::Text("Cube Controls");
        //ImGui::SliderFloat3("Position", glm::value_ptr(controlledPosition), -1.5f, 1.5f);
        //ImGui::SliderFloat3("Direction", glm::value_ptr(controlledDirection), -1.0f, 1.0f);
        //ImGui::Separator();
        //ImGui::Text("Light Cube Controls");
        //ImGui::SliderFloat3("Light Position", glm::value_ptr(lightCubePosition), -2.0f, 2.0f);
        //ImGui::ColorEdit3("Light Color", glm::value_ptr(*lightPoint.getColor()));
        //ImGui::Separator();
        ImGui::Text("Camera Controls");
        ImGui::Text("Current camera position (%f, %f, %f)", cameraOrbitPosition.x, cameraOrbitPosition.y, cameraOrbitPosition.z);
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
        if (ImGui::Button("Camera orbit (far)"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 1.0f, &cameraOrbitPosition);
            bindInputToController(input, cameraController);
        }
        if (ImGui::Button("Camera orbit (close)"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 0.2f, &cameraOrbitPosition);
            bindInputToController(input, cameraController);
        }
        ImGui::Separator();
        ImGui::Text("Frame number %u", nFrame);
        ImGui::Text("FPS: %f", 1 / deltaTime);
        ImGui::End();

        runtimeModelEditor.update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // check and call events and swap the buffers

        window.update();

        nFrame++;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    if (cameraController != nullptr)
        delete cameraController;

    return (shouldExit ? 0 : 1); // 0 si se cierra correctamente, 1 si se cierra mal
}