#pragma once

// Controllers
#include "EntityControllerSystem.h"

#include "rendering/engine.h"
#include "rendering/editor.h"

#include "game/gamemodes.h"
#include "game/BaseGameState.h"
#include "game/detectionBox.h"
#include "coroutine.h"

class Game
{
public:
    Game(Window* window, GAMEMODE gamemode, int numPlayers);
    ~Game();

    int startGameLoop();

private:
    Window* m_window;

    BaseGameState* m_gameState;

    Rendering::RenderEngine3D* m_renderEngine;
    std::vector<Rendering::Shader*> m_shaders;
    Camera m_camera;

    std::vector<int> m_barRenderIndexes;
    std::vector<int> m_ballRenderIndexes;
    std::vector<int> m_lightIndexes;

    std::vector<bool> m_pocketed;
    float m_teamOffSet1;
    float m_teamOffSet2;

    float m_Width;
    float m_Height;
    float m_resFix;

    void initializeRenderObjects();
    void initializeRenderLights();
    void initializeBasicInputs();
    void initializeDetectionBoxes();
    void initializeHUD();

    void processShadows();

    CameraController* m_currentCameraController;

    EntityControllerSystem* m_physicsEngine;


    std::vector<DetectionBox> m_detectionBoxes;


    void drawDebugUI(unsigned int nFrame, double deltaTime, Input* input, glm::vec3* focusedBallPosition, Rendering::RuntimeModelEditor* runtimeModelEditor);
    void drawHUD();


    // game functions
    bool m_shouldExit = false;
    bool m_isMoveDone = false;    // se inicia en cada turno a false, cuando el jugador mueve una bola se pone en true
    void playerTurn(Coroutine* coro);
    std::function<void(float)> m_pushBallFunction;
    int m_power = 1;
    int m_maxPower = 4;
    std::function<void(float)> m_upPowerFunction;
    std::function<void(float)> m_downPowerFunction;
    void winCoroutine(Coroutine* coro);

    std::vector<glm::vec3> m_playerColors{ glm::vec3(0xA2/255.0, 0x4B/255.0, 0xFF/255.0), glm::vec3(0x7C / 255.0, 0xFC / 255.0, 0x00 / 255.0) };  // #A24BFF, #7CFC00
    glm::vec3 m_currentColor = glm::vec3(0x44 / 255.0);
};