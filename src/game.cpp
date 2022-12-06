#include "game.h"

int Game(Window& window) {

    // enables z-buffer
    glEnable(GL_DEPTH_TEST);

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

    #ifdef DEBUG_SHADER
    Shader debugShader("shaders/debug.vert", "shaders/debug.frag");
    if(!debugShader.compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        return 1;
    }
    #endif


    /////////// IMGUI init ///////////

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool drawTriangles = true;

    glm::vec3 controlledPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 controlledDirection = glm::vec3(0.5f, 1.0f, 0.0f);

    /////////// INPUT init ///////////

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
    glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f); /// this should be updated when window changes size


    enum CameraType {
        FLY,
        FPS,
        ORBIT
    };
    CameraType currentType = ORBIT;

    CameraController* cameraController = new CameraControllerOrbit(&camera, 2.5f, 1.0f, &controlledPosition);
    bindInputToController(input, cameraController);


    // here goes the models
    // Rendering::Model poolTableModel("models/pooltable/Pool table Cavicchi Leonardo 9FT N300818.3ds");
    Rendering::Model poolTableModel("models/pool_table/scene.gltf");
    Rendering::Object poolTable(&poolTableModel);

    

    /////////// LIGHTS ///////////

    glm::vec3 lightCubePosition = glm::vec3( 0.0f,  1.2f,  0.0f);
    glm::vec3 lightCubeColor = glm::vec3(1.0f, 1.0f, 1.0f);

    LOG_DEBUG("Loading lights...");
    Rendering::LightPoint lightPoint(0);
    LOG_DEBUG("Loaded lights");

    //////////// FISICAS ////////////
	///-----initialization_start-----

    EntityControllerSystem ECS(CLASSIC, input, &camera);


    ///// INPUTS FOR BALL //////

    btDiscreteDynamicsWorld* dynamicsWorld = ECS.getDynamicsWorld();
    int ballsIndex = ECS.getBallsIndex();

    input->setKeyAction(PUSH_BALL, GLFW_KEY_F);
    input->setActionFunction(PUSH_BALL, [&dynamicsWorld, &camera, &ballsIndex](float deltaTime) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex];
        btRigidBody* body = btRigidBody::upcast(obj);
        body->setActivationState(ACTIVE_TAG);
        glm::vec3 front = camera.getPlaneFront();
        front = front * 4.0f;
        body->setLinearVelocity(btVector3(front.x, body->getLinearVelocity().y(), front.z));
        });


    input->setKeyAction(ACCELERATE_BALL, GLFW_KEY_E);
    input->setActionFunction(ACCELERATE_BALL, [dynamicsWorld, &camera, &ballsIndex](float deltaTime) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex];
        btRigidBody* body = btRigidBody::upcast(obj);
        body->setActivationState(ACTIVE_TAG);
        glm::vec3 front = camera.getPlaneFront();
        front = front * 4.0f;
        body->setLinearVelocity(btVector3(body->getLinearVelocity().x() * (1.01 + deltaTime), body->getLinearVelocity().y(), body->getLinearVelocity().z() * (1.01 + deltaTime)));
        });

    

    unsigned int nFrame = 0;
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while (!window.shouldClose())
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
        ECS.StepSimulation(deltaTime, 100, 0.001f);

        glm::mat4 view = camera.getViewMatrix();

        /////////// RENDERING ///////////
        if (drawTriangles)
        {
            lightPoint.setPosition(lightCubePosition);
            lightPoint.draw(&lightShader, view, projection);
            lightPoint.updateShader(&modelShader);

            glm::vec3 poolTablePosition = glm::vec3(0.0f);
            poolTable.setPosition(poolTablePosition);
            glm::vec3 poolTableOrientation = glm::vec3(0.0f, 1.0f, 0.0f);
            poolTable.setOrientation(0.0f, poolTableOrientation);
            poolTable.setScaling(0.1245);

            poolTable.draw(&modelShader, view, projection, camera.getPosition());

            ECS.draw(&modelShader, &camera, projection, controlledPosition, &debugShader);
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
        if (ImGui::Button("Camera orbit (far)"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 1.0f, &controlledPosition);
            bindInputToController(input, cameraController);
        }
        if (ImGui::Button("Camera orbit (close)"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 0.2f, &controlledPosition);
            bindInputToController(input, cameraController);
        }
        ImGui::Separator();
        ImGui::Text("Frame number %u", nFrame);
        ImGui::Text("FPS: %f", 1 / deltaTime);
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