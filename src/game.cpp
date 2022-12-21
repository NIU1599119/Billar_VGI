#include "game.h"

#ifdef DEBUG
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

#include "game/ClassicState.h"
#include "game/CarambolaState.h"

#include "menu/rendering/SpriteRenderer.h"
#include "menu/resource_manager.h"

// Matrix
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl_utils.h"

SpriteRenderer* RendererGame;

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

    m_physicsEngine = new EntityControllerSystem(m_gameState, m_renderEngine, m_ballRenderIndexes);

    LOG_INFO("Initialized the physics engine");

    initializeDetectionBoxes();

    // variables necesarias para las funciones
    Input* input = m_window->getInput();
    btDiscreteDynamicsWorldMt** p_dynamicsWorld = m_physicsEngine->getDynamicsWorld();
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
    Rendering::Model* roomModel = Rendering::createModel("models/room/Room.obj");
    int roomRenderID = m_renderEngine->createObject(roomModel, 1.0);
    m_renderEngine->updateObject(roomRenderID, glm::vec3(0.0), glm::quat(1.0, 0.0, 0.0, 0.0));
    m_renderEngine->updateObjectScaling(roomRenderID, glm::vec3(2.0, 4.0, 2.0));
    m_barRenderIndexes.push_back(roomRenderID);

    int roomRenderID2 = m_renderEngine->createObject(roomModel,1.0);
    m_renderEngine->updateObject(roomRenderID2, glm::vec3(0.0, 4.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0));
    m_renderEngine->updateObjectScaling(roomRenderID2, 2.0);
    m_barRenderIndexes.push_back(roomRenderID2);

    //add pub_counter
    int barraBarRenderID = m_renderEngine->createObject(std::string("models/pub_counter/scene.gltf"), 1.0);
    m_renderEngine->updateObject(barraBarRenderID, glm::vec3(2.512, 0.0, 1.615), glm::quat(0.5, -0.5, 0.5, 0.5));
    m_renderEngine->updateObjectScaling(barraBarRenderID, glm::vec3(0.2, 0.2, 0.25));
    m_barRenderIndexes.push_back(barraBarRenderID);

    //add wood_door
    int doorRenderID = m_renderEngine->createObject(std::string("models/wooden_door/scene.gltf"), 1.0);
    m_renderEngine->updateObject(doorRenderID, glm::vec3(-3.96, 0.0, 3.25), glm::quat(0.5, -0.5, -0.5, -0.5));
    m_renderEngine->updateObjectScaling(doorRenderID, glm::vec3(0.35, 0.2, 0.35));
    m_barRenderIndexes.push_back(doorRenderID);

    //add wood_table
    Rendering::Model* table = Rendering::createModel("models/wooden_bar_table/scene.gltf");
    int tableRenderID = m_renderEngine->createObject(table, 1.0);
    m_renderEngine->updateObject(tableRenderID, glm::vec3(0.0, 0.323, -2.385), glm::quat(1.0, 0.0, 0.0, 0.0));
    m_renderEngine->updateObjectScaling(tableRenderID, glm::vec3(0.4));
    m_barRenderIndexes.push_back(tableRenderID);

    int tableRenderID2 = m_renderEngine->createObject(table, 1.0);
    m_renderEngine->updateObject(tableRenderID2, glm::vec3(2.0, 0.323, -2.385), glm::quat(1.0, 0.0, 0.0, 0.0));
    m_renderEngine->updateObjectScaling(tableRenderID2, glm::vec3(0.4));
    m_barRenderIndexes.push_back(tableRenderID2);

    int tableRenderID3 = m_renderEngine->createObject(table, 1.0);
    m_renderEngine->updateObject(tableRenderID3, glm::vec3(-2.0, 0.323, -2.385), glm::quat(1.0, 0.0, 0.0, 0.0));
    m_renderEngine->updateObjectScaling(tableRenderID3, glm::vec3(0.4));
    m_barRenderIndexes.push_back(tableRenderID3);

    //add chairs mesa 1
    Rendering::Model* chair = Rendering::createModel("models/chair/scene.gltf");
    int chairRenderID = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID, glm::vec3(-2.0, 0.0, -2.85), glm::quat(-0.0, 0.0, 0.707107, 0.707107));
    m_renderEngine->updateObjectScaling(chairRenderID, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID);

    int chairRenderID2 = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID2, glm::vec3(-1.5, 0.0, -2.385), glm::quat(0.5, -0.5, 0.5, 0.5));
    m_renderEngine->updateObjectScaling(chairRenderID2, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID2);

    int chairRenderID3 = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID3, glm::vec3(-2.434, 0.0, -2.385), glm::quat(0.5, -0.5, -0.5, -0.5));
    m_renderEngine->updateObjectScaling(chairRenderID3, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID3);

    //add chairs mesa 2
    int chairRenderID4 = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID4, glm::vec3(0.0, 0.0, -2.85), glm::quat(-0.0, 0.0, 0.707107, 0.707107));
    m_renderEngine->updateObjectScaling(chairRenderID4, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID4);

    int chairRenderID5 = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID5, glm::vec3(0.434, 0.0, -2.385), glm::quat(0.5, -0.5, 0.5, 0.5));
    m_renderEngine->updateObjectScaling(chairRenderID5, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID5);

    int chairRenderID6 = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID6, glm::vec3(-0.434, 0.0, -2.385), glm::quat(0.5, -0.5, -0.5, -0.5));
    m_renderEngine->updateObjectScaling(chairRenderID6, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID6);

    //add chairs mesa 3
    int chairRenderID7 = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID7, glm::vec3(2.0, 0.0, -2.85), glm::quat(-0.0, 0.0, 0.707107, 0.707107));
    m_renderEngine->updateObjectScaling(chairRenderID7, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID7);

    int chairRenderID8 = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID8, glm::vec3(1.5, 0.0, -2.385), glm::quat(0.5, -0.5, -0.5, -0.5));
    m_renderEngine->updateObjectScaling(chairRenderID8, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID8);

    int chairRenderID9 = m_renderEngine->createObject(chair, 1.0);
    m_renderEngine->updateObject(chairRenderID9, glm::vec3(2.434, 0.0, -2.385), glm::quat(0.5, -0.5, 0.5, 0.5));
    m_renderEngine->updateObjectScaling(chairRenderID9, glm::vec3(0.8));
    m_barRenderIndexes.push_back(chairRenderID9);
    

    switch (m_gameState->getGamemode())
    {
    case CLASSIC:
    {
        // add the table
        int poolRenderID = m_renderEngine->createObject(std::string("models/pool-table/table.obj"), 0.7);
        m_renderEngine->updateObject(poolRenderID, glm::vec3(0.0,0.064,0.0), glm::quat(1.0, 0.0, 0.0, 0.0));
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


void Game::drawDebugUI(unsigned int nFrame, double deltaTime, Input* input, glm::vec3* focusedBallPosition, Rendering::RuntimeModelEditor* runtimeModelEditor)
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

    switch (m_gameState->getGamemode())
    {
    case CLASSIC:
    {
        ClassicState* classicState = (ClassicState*)m_gameState;
        ImGui::Begin("Classic mode debug window");
        ImGui::Text("Teams:");

        std::vector<CLASSIC_BALL_TYPES> teams = classicState->getTeams();
        ImGui::Indent(8);
        for (int i = 0; i < teams.size(); i++)
        {
            std::string ballType = "NOT SET";
            if (teams[i] == STRIPED)
                ballType = "STRIPED (ralladas)";
            else if (teams[i] == SOLID)
                ballType = "SOLID (lisas)";

            std::string team = "Team " + std::to_string(i) + " : " + ballType;
            ImGui::Text(team.c_str());
        }
        ImGui::Unindent(8);
        ImGui::Separator();
        ImGui::Text("Balls:");

        std::vector<bool> pocketed = classicState->getPocketedBalls();

        ImGui::Indent(8);
        ImGui::Text("SOLID");
        ImGui::Indent(8);
        std::string solidPockets;
        for (int i = 1; i < 8; i++)
        {
            if (pocketed[i])
                solidPockets = solidPockets + "1 ";
            else
                solidPockets = solidPockets + "0 ";
        }
        ImGui::Text(solidPockets.c_str());
        ImGui::Unindent(8);
        ImGui::Text("EIGHT");
        ImGui::Indent(8);
        {
            if (pocketed[8])
                ImGui::Text("1");
            else
                ImGui::Text("0");
        }
        ImGui::Unindent(8);
        ImGui::Text("STRIPED");
        ImGui::Indent(8);
        std::string stripedPockets;
        for (int i = 9; i < 16; i++)
        {
            if (pocketed[i])
                stripedPockets = stripedPockets + "1 ";
            else
                stripedPockets = stripedPockets + "0 ";
        }
        ImGui::Text(stripedPockets.c_str());
        ImGui::Unindent(8);
        ImGui::Unindent(8);

        ImGui::End();

        ImGui::Begin("Holes");

        for (int i = 0; i < m_detectionBoxes.size(); i++)
        {
            ImGui::Text("Hole %d", i);
            std::vector<int> balls = m_detectionBoxes[i].getDetectedBallIDs();;
            for (int j = 0; j < balls.size(); j++)
            {
                ImGui::Text("Ball %d", balls[j]);
            }
        }
        ImGui::End();

        break;
    }
    case CARAMBOLA:
    {
        ImGui::Begin("Carambola mode debug window");
        ImGui::Text("Not implemented");
        ImGui::End();
        break;
    }
    default:
        break;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void Game::initializeDetectionBoxes()
{
    EntityControllerSystem** p_physicsEngine = &m_physicsEngine;
    BaseGameState** p_gameState = &m_gameState;
    std::function<void(int)> pocketCallback = [p_physicsEngine, p_gameState](int ballID) {

        (*p_physicsEngine)->resetBall(ballID);
        if ((*p_gameState)->getGamemode() == CLASSIC)
        {
            bool shouldResetBall = ((ClassicState*)(*p_gameState))->pocketBall(ballID);
            if (shouldResetBall) return;
        }
        (*p_physicsEngine)->disableBall(ballID);
    };
    std::function<void(int)> outOfBoundsCallback = [p_physicsEngine](int ballID) {
        (*p_physicsEngine)->resetBall(ballID);
    };
    // floor
    m_detectionBoxes.emplace_back(m_renderEngine, m_physicsEngine, outOfBoundsCallback, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(10.0, 1.2, 10.0));

    switch (m_gameState->getGamemode())
    {
    case CLASSIC:
        // holes
        /// middle
        m_detectionBoxes.emplace_back(m_renderEngine, m_physicsEngine, pocketCallback, glm::vec3( 0.0f,  0.7f,  0.66f), glm::vec3(0.2, 0.1, 0.2));
        m_detectionBoxes.emplace_back(m_renderEngine, m_physicsEngine, pocketCallback, glm::vec3( 0.0f,  0.7f, -0.66f), glm::vec3(0.2, 0.1, 0.2));
        /// corner
        m_detectionBoxes.emplace_back(m_renderEngine, m_physicsEngine, pocketCallback, glm::vec3( 1.15f, 0.7f, -0.6f), glm::vec3(0.2, 0.1, 0.2));
        m_detectionBoxes.emplace_back(m_renderEngine, m_physicsEngine, pocketCallback, glm::vec3( 1.15f, 0.7f,  0.6f), glm::vec3(0.2, 0.1, 0.2));
        m_detectionBoxes.emplace_back(m_renderEngine, m_physicsEngine, pocketCallback, glm::vec3(-1.15f, 0.7f, -0.6f), glm::vec3(0.2, 0.1, 0.2));
        m_detectionBoxes.emplace_back(m_renderEngine, m_physicsEngine, pocketCallback, glm::vec3(-1.15f, 0.7f,  0.6f), glm::vec3(0.2, 0.1, 0.2));
        break;
    default:
        break;
    }
}

void Game::initializeHUD()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    float Width = m_window->getWidth();
    float Height = m_window->getHeight();
    float resFix = Width / 1920;

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width),
        static_cast<float>(Height), 0.0f, -1.0f, 1.0f);


    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls

    RendererGame = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    ResourceManager::LoadTexture("textures/hud/hud.png", true, "hud");
    ResourceManager::LoadTexture("textures/hud/1.png", true, "1");
    ResourceManager::LoadTexture("textures/hud/2.png", true, "2");
    ResourceManager::LoadTexture("textures/hud/3.png", true, "3");
    ResourceManager::LoadTexture("textures/hud/4.png", true, "4");
    ResourceManager::LoadTexture("textures/hud/5.png", true, "5");
    ResourceManager::LoadTexture("textures/hud/6.png", true, "6");
    ResourceManager::LoadTexture("textures/hud/7.png", true, "7");
    ResourceManager::LoadTexture("textures/hud/9.png", true, "9");
    ResourceManager::LoadTexture("textures/hud/10.png", true, "10");
    ResourceManager::LoadTexture("textures/hud/11.png", true, "11");
    ResourceManager::LoadTexture("textures/hud/12.png", true, "12");
    ResourceManager::LoadTexture("textures/hud/13.png", true, "13");
    ResourceManager::LoadTexture("textures/hud/14.png", true, "14");
    ResourceManager::LoadTexture("textures/hud/15.png", true, "15");
    ResourceManager::LoadTexture("textures/hud/p1.png", true, "p1");
    ResourceManager::LoadTexture("textures/hud/p2.png", true, "p2");

    m_teamOffSet1 = 0.0f;
    m_teamOffSet2 = 960.0f;

    m_Width = m_window->getWidth();
    m_Height = m_window->getHeight();
    m_resFix = Width / 1920;
}

void Game::drawHUD()
{
    //// rendering HUD
    ClassicState* classicState = (ClassicState*)m_gameState;;
    m_pocketed = classicState->getPocketedBalls();


    std::vector<CLASSIC_BALL_TYPES> teams = classicState->getTeams();

    bool print = false;

    if (teams[0] == STRIPED)
    {
        print = true;
        m_teamOffSet1 = 960.0f;
        m_teamOffSet2 = 0.0f;
    }
    else if (teams[0] == SOLID)
    {
        print = true;
        m_teamOffSet1 = 0.0f;
        m_teamOffSet2 = 960.0f;
    }

    if (print)
    {
        for (int i = 1; i < 8; i++)
        {
            if (!m_pocketed[i])
            {
                RendererGame->DrawSprite(ResourceManager::GetTexture(std::to_string(i)),
                    glm::vec2(((100.0f + (85.0f * i)) + m_teamOffSet1) * m_resFix, 50.0f * m_resFix), glm::vec2(85 * m_resFix, 85 * m_resFix), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

            }
        }
        for (int i = 9; i < 16; i++)
        {
            if (!m_pocketed[i])
            {
                RendererGame->DrawSprite(ResourceManager::GetTexture(std::to_string(i)),
                    glm::vec2(((100.0f + (85.0f * (i - 8))) + m_teamOffSet2) * m_resFix, 50.0f * m_resFix), glm::vec2(85 * m_resFix, 85 * m_resFix), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

            }
        }
    }

    if (classicState->getCurrentPlayer() == 0)
    {
        RendererGame->DrawSprite(ResourceManager::GetTexture("p1"),
            glm::vec2(810.0f * m_resFix, 900.0f * m_resFix), glm::vec2(300 * m_resFix, 150 * m_resFix), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
    else
    {
        RendererGame->DrawSprite(ResourceManager::GetTexture("p2"),
            glm::vec2(810.0f * m_resFix, 900.0f * m_resFix), glm::vec2(300 * m_resFix, 150 * m_resFix), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    RendererGame->DrawSprite(ResourceManager::GetTexture("hud"),
        glm::vec2(0.0f * m_resFix, 0.0f * m_resFix), glm::vec2(1920 * m_resFix, 1080 * m_resFix), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

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

    m_gameState->processTurn();
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

    Coroutine playerTurnCoroutine;

    m_shouldExit = false;

    initializeBasicInputs();

    initializeHUD();

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

        for (int i = 0; i < m_detectionBoxes.size(); i++)
        {
            m_detectionBoxes[i].checkBallInside(m_gameState->getGamemode());
            m_detectionBoxes[i].update();
        }

        //// rendering update

        m_renderEngine->updateShaderView();
        m_renderEngine->drawAll();

        m_renderEngine->drawLights();

        drawHUD();

        
        #ifdef DEBUG_SHADER
        std::vector<Rendering::CollisionBox>* p_rigidObjects = m_physicsEngine->getCollisionsBoxPtr();
        for (int i = 0; i < (*p_rigidObjects).size(); i++)
        {
            //rigidObjects[i].draw(&debugShader, view, projection);
            (*p_rigidObjects)[i].draw(m_renderEngine->getDebuggingShader());
        }
        #endif

        #ifdef DEBUG
        drawDebugUI(nFrame, deltaTime, m_window->getInput(), &focusedBallPosition, &runtimeModelEditor);
        #endif

        m_window->update();

        nFrame++;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

    }

    return (m_shouldExit ? 0 : 1); // 0 si se cierra correctamente, 1 si se cierra de otra forma (crash, forzado, ...)
}

