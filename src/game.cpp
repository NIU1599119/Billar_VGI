#include "game.h"

#ifdef DEBUG
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

#include "game/ClassicState.h"
#include "game/CarambolaState.h"

Game::Game(Window* window, GAMEMODE gamemode, int numPlayers)
: m_window(window)
{
    // inicializacion del modo de juego
    switch (gamemode)
    {
    case CLASSIC:
        m_gameState = new ClassicState(numPlayers);
        break;
    case CARAMBOLA:
        m_gameState = new CarambolaState(numPlayers);
        break;
    case FREE_SHOTS:
        LOG_ERROR("No hay tiros libres todavia");
        ASSERT(false);
        break;
    default:
        LOG_ERROR("Modo de juego desconocido");
        ASSERT(false);
        break;
    }

    // inicializacion de los shaders
    Rendering::Shader* modelShader = new Rendering::Shader("shaders/model.vert", "shaders/model.frag");
    if(!modelShader->compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        ASSERT(false);
    }

    Rendering::Shader* lightShader = new Rendering::Shader("shaders/light.vert", "shaders/light.frag");
    if(!lightShader->compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        ASSERT(false);
    }

    Rendering::Shader* debugShader = new Rendering::Shader("shaders/debug.vert", "shaders/debug.frag");
    if(!debugShader->compileShaders())
    {
        LOG_ERROR("Failed compiling debugShader");
        ASSERT(false);
    }

    m_shaders.push_back(modelShader);
    m_shaders.push_back(lightShader);
    m_shaders.push_back(debugShader);

    LOG_INFO("Shaders compiled");

    // camara
    m_camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_renderEngine = new Rendering::RenderEngine3D(&m_camera, modelShader, lightShader, debugShader);

    initializeRenderObjects();
    initializeRenderLights();

    LOG_INFO("Initialized all render objects");

    //////////// FISICAS ////////////
    ///-----initialization_start-----

    m_physicsEngine = new EntityControllerSystem(gamemode, m_renderEngine, m_ballRenderIndexes);

    LOG_INFO("Initialized the physics engine");

    // variables necesarias para las funciones
    Input* input = m_window->getInput();
    btDiscreteDynamicsWorld** p_dynamicsWorld = m_physicsEngine->getDynamicsWorld();
    Camera* p_camera = &m_camera;
    bool* p_isMoveDone = &m_isMoveDone;
    int currentBallToShoot = m_gameState->getPlayerBallID();
    currentBallToShoot = m_physicsEngine->getBallIdx(currentBallToShoot);

    m_pushBallFunction = [p_dynamicsWorld, p_camera, currentBallToShoot, p_isMoveDone](float deltaTime){
        btCollisionObject* obj = (*p_dynamicsWorld)->getCollisionObjectArray()[currentBallToShoot];
        btRigidBody* body = btRigidBody::upcast(obj);
        body->setActivationState(ACTIVE_TAG);
        glm::vec3 front = p_camera->getPlaneFront();
        front = front * 3.0f;
        body->setLinearVelocity(btVector3(front.x, body->getLinearVelocity().y(), front.z));
        *p_isMoveDone = true;
    };
    LOG_INFO("Initialized the game functions");
    

    LOG_INFO("Game engine is initialized");
}

Game::~Game()
{
    if (m_physicsEngine != nullptr) delete m_physicsEngine;
    m_physicsEngine = nullptr;

    if (m_renderEngine != nullptr) delete m_renderEngine;
    m_renderEngine = nullptr;

    for (int i = 0; i < m_shaders.size(); i++)
    {
        if (m_shaders[i] != nullptr) delete m_shaders[i];
        m_shaders[i] = nullptr;
    }

    if (m_gameState != nullptr) delete m_gameState;
    m_gameState = nullptr;
}



void Game::initializeRenderObjects()
{   
    //add the room
  /*  int roomRenderID = m_renderEngine->createObject(std::string("models/room/Room.obj"));
    m_renderEngine->updateObject(roomRenderID, glm::vec3(0.0), glm::quat(1.0, 0.0, 0.0, 0.0));
    m_renderEngine->updateObjectScaling(roomRenderID, 2.0);
    m_barRenderIndexes.push_back(roomRenderID);

    int roomRenderID2 = m_renderEngine->createObject(std::string("models/room/Room.obj"));
    m_renderEngine->updateObject(roomRenderID, glm::vec3(0.0), glm::quat(1.0, 0.0, 0.0, 0.0));
    m_renderEngine->updateObjectScaling(roomRenderID, 2.0);
    m_barRenderIndexes.push_back(roomRenderID);*/


    switch (m_gameState->getGamemode())
    {
    case CLASSIC:
    {
        // add the table
        int poolRenderID = m_renderEngine->createObject(std::string("models/pool_table/scene.gltf"), 0.109025);
        m_renderEngine->updateObject(poolRenderID, glm::vec3(0.0), glm::quat(1.0, 0.0, 0.0, 0.0));
        m_barRenderIndexes.push_back(poolRenderID);


        // add the balls
        int cueBallRenderID = m_renderEngine->createObject(std::string("models/billiard-balls/ballwhite.obj"), 0.05715);
        m_ballRenderIndexes.push_back(cueBallRenderID); // 0 is CUE ball

        for (int i = 1; i < 16; i++)
        {
            int iBallID = m_renderEngine->createObject(std::string("models/billiard-balls/ball" + std::to_string(i) + ".obj"), 0.05715);
            m_ballRenderIndexes.push_back(iBallID);
        }

        break;
    }
    case CARAMBOLA:
        // de momento no tenemos nada
        // meter la mesa

        // meter las bolas

        break;
    case FREE_SHOTS:
        // de momento no tenemos nada
        break;
    default:
        break;
    }

    /// aqui va el codigo que carga todo el bar y pone los muebles en sus posiciones

}


void Game::initializeRenderLights()
{
    for (int i = 0; i < 5; i++)
    {
        int idx = m_renderEngine->addLight();
        m_lightIndexes.push_back(idx);
        m_renderEngine->setLightPosition(idx, glm::vec3(-1.0 + (i / 2.0), 2.5, 0.0));
        m_renderEngine->setLightColor(idx, glm::vec3(1.0, 1.0, 1.0));
        m_renderEngine->setLightPolinomial(idx, 2.0, 0.09, 0.0032);
    }
}

void Game::initializeBasicInputs()
{
    Window** p_window = &m_window;
    Input* input = m_window->getInput();
    bool* p_shouldExit = &m_shouldExit;

    input->setKeyAction(EXIT, GLFW_KEY_ESCAPE, false);
    input->setActionFunction(EXIT, [p_shouldExit](float deltaTime) { LOG_INFO("Exiting Game"); *p_shouldExit = true; });

    #ifdef DEBUG
    input->setKeyAction(SWITCH_MOUSE, GLFW_KEY_M, false);
    input->setActionFunction(SWITCH_MOUSE, [p_window, input](float deltaTime) {
        if (input->mouseIsCaptured())
        {
            glfwSetInputMode((*p_window)->getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            input->uncaptureMouse();
        }
        else
        {
            glfwSetInputMode((*p_window)->getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);    // hides cursor and sets it to middle
            input->captureMouse();
        }
        });
    #else
    glfwSetInputMode((*p_window)->getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    input->captureMouse();
    #endif


    input->setKeyAction(MOVE_FORWARDS, GLFW_KEY_W);
    input->setKeyAction(MOVE_BACKWARDS, GLFW_KEY_S);
    input->setKeyAction(MOVE_LEFT, GLFW_KEY_A);
    input->setKeyAction(MOVE_RIGHT, GLFW_KEY_D);
}


void Game::drawDebugUI(unsigned int nFrame, double deltaTime, Input* input, glm::vec3* focusedBallPosition, Rendering::RuntimeModelEditor* runtimeModelEditor, DetectionBox& hole)
{
    /////////// ImGui ///////////

    ImGui::Begin("Debug Window");
    ImGui::Text("Camera Controls");
    glm::vec3 currentCameraPosition = m_camera.getPosition();
    ImGui::Text("Current camera position (%f, %f, %f)", currentCameraPosition.x, currentCameraPosition.y, currentCameraPosition.z);
    if (ImGui::Button("Camera fly"))
    {
        unbindInputToController(input);
        if (m_currentCameraController != nullptr)
            delete m_currentCameraController;
        m_currentCameraController = new CameraControllerFly(&m_camera);
        bindInputToController(input, m_currentCameraController);
    }
    if (ImGui::Button("Camera fps"))
    {
        unbindInputToController(input);
        if (m_currentCameraController != nullptr)
            delete m_currentCameraController;
        m_currentCameraController = new CameraControllerFps(&m_camera);
        bindInputToController(input, m_currentCameraController);
    }
    if (ImGui::Button("Camera orbit (far)"))
    {
        unbindInputToController(input);
        if (m_currentCameraController != nullptr)
            delete m_currentCameraController;
        m_currentCameraController = new CameraControllerOrbit(&m_camera, 2.5f, 1.0f, focusedBallPosition);
        bindInputToController(input, m_currentCameraController);
    }
    if (ImGui::Button("Camera orbit (close)"))
    {
        unbindInputToController(input);
        if (m_currentCameraController != nullptr)
            delete m_currentCameraController;
        m_currentCameraController = new CameraControllerOrbit(&m_camera, 2.5f, 0.2f, focusedBallPosition);
        bindInputToController(input, m_currentCameraController);
    }
    ImGui::Separator();
    ImGui::Text("Frame number %u", nFrame);
    ImGui::Text("FPS: %f", 1 / deltaTime);
    ImGui::Separator();
    ImGui::Text("Game status");
    ImGui::Text("Current player is %d", m_gameState->getCurrentPlayer());
    ImGui::End();

    runtimeModelEditor->update();

    ImGui::Begin("Holes");
    glm::vec3 holePos = hole.getPosition();
    glm::vec3 holeSize = hole.getSize();
    ImGui::DragFloat3("position", glm::value_ptr(holePos), 0.001);
    ImGui::DragFloat3("size", glm::value_ptr(holeSize), 0.001);
    hole.setPosition(holePos);
    hole.setSize(holeSize);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}


void Game::playerTurn(Coroutine* coro)
{
    // gracias a bigrando420 se puede simplificar un monton este codigo
    // los yields hacen como un checkpoint y salen de la funcion
    // cuando hay condicion el checkpoint se queda bloqueado hasta que la condicion se cumple
    // al final se tiene que hacer un CoroutineEnd o un CoroutineReset, el end hace que no se pueda volver a ejecutar hasta que
    //          coro->line = 0, el reset hace que coro->line = 0 y entonces se puede volver a ejecutar otro bucle de juego justo cuando se acaba este

    // variables necesarias en toda la corutina
    Input* input = m_window->getInput();

    CoroutineBegin(coro);
    // asign the input to the player
    // LOG_DEBUG("Begin corutine");
    input->setKeyAction(PUSH_BALL, GLFW_KEY_F, false);
    input->setActionFunction(PUSH_BALL, m_pushBallFunction);

    // player has to throw the ball so we wait for it
    CoroutineYieldUntil(coro, m_isMoveDone);
    // when player does move, he can no longer shoot again
    // LOG_DEBUG("Player has moved");
    input->removeActionFunction(PUSH_BALL);
    input->removeKeyAction(GLFW_KEY_F);

    // now that the move is done we wait for the simulation to be static
    while(!m_physicsEngine->isStatic())
    {
        CoroutineYield(coro);   // esto hace que se detenga la ejecucion hasta que haya que salir del bucle
                                // se ve raro pero el bucle no hace nada
                                // tambien se podria poner como: CoroutineYieldUntil(coro, m_physicsEngine->isStatic())
    }
    // LOG_DEBUG("World has stopped");

    // now that the simulation is static the next turn can go on

    m_gameState->nextTurn();
    m_isMoveDone = false;
    // LOG_DEBUG("Corutine has ended, next turn\n");

    CoroutineReset(coro);
}

int Game::startGameLoop()
{
    initializeBasicInputs();
    glm::vec3 focusedBallPosition;
    m_currentCameraController = new CameraControllerOrbit(&m_camera, 2.5, 0.5, &focusedBallPosition);
    bindInputToController(m_window->getInput(), m_currentCameraController);

    unsigned int nFrame = 0;
    double deltaTime = 0.0f;	// Time between current frame and last frame
    double lastFrame = 0.0f; // Time of last frame

    #ifdef DEBUG
    /////////// IMGUI init ///////////
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // debug imgui tools
    Rendering::RuntimeModelEditor runtimeModelEditor(m_renderEngine);
    #endif

    DetectionBox hole(m_renderEngine, m_physicsEngine, glm::vec3(0.0f, 0.789f, 1.0f), glm::vec3(0.1, 0.1, 0.1));


    Coroutine playerTurnCoroutine;

    m_shouldExit = false;

    initializeBasicInputs();

    while (!m_window->shouldClose() && !m_shouldExit)
    {
        GL(glClearColor(0.1f, 0.1f, 0.15f, 1.0f));
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        #ifdef DEBUG
        // ImGui newframe
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!io.WantCaptureMouse)
        {
            // blocks anything here if mouse is over imgui
            m_window->getInput()->enableKeyboard();
            playerTurn(&playerTurnCoroutine);
            m_window->processInput(deltaTime);
        }
        else { m_window->getInput()->disableKeyboard(); };
        #else
        playerTurn(&playerTurnCoroutine);
        m_window->processInput(deltaTime);
        #endif

        m_physicsEngine->update(deltaTime, &focusedBallPosition);
        
        m_currentCameraController->update();

        hole.checkBallInside(m_gameState->getGamemode());
        hole.update();

        //// rendering update

        m_renderEngine->updateShaderView();
        m_renderEngine->drawAll();

        m_renderEngine->drawLights();

        #ifdef DEBUG_SHADER
        std::vector<Rendering::CollisionBox>* p_rigidObjects = m_physicsEngine->getCollisionsBoxPtr();
        for (int i = 0; i < (*p_rigidObjects).size(); i++)
        {
            //rigidObjects[i].draw(&debugShader, view, projection);
            (*p_rigidObjects)[i].draw(m_renderEngine->getDebuggingShader());
        }
        #endif

        #ifdef DEBUG
        drawDebugUI(nFrame, deltaTime, m_window->getInput(), &focusedBallPosition, &runtimeModelEditor, hole);
        #endif

        m_window->update();

        nFrame++;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

    }

    return (m_shouldExit ? 0 : 1); // 0 si se cierra correctamente, 1 si se cierra de otra forma (crash, forzado, ...)
}

