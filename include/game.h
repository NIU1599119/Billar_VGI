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

    void initializeRenderObjects();
    void initializeRenderLights();
    void initializeBasicInputs();

    CameraController* m_currentCameraController;

    EntityControllerSystem* m_physicsEngine;

    void drawDebugUI(unsigned int nFrame, double deltaTime, Input* input, glm::vec3* focusedBallPosition, Rendering::RuntimeModelEditor* runtimeModelEditor, DetectionBox& hole);


    // game functions
    bool m_shouldExit = false;
    bool m_isMoveDone = false;    // se inicia en cada turno a false, cuando el jugador mueve una bola se pone en true
    void playerTurn(Coroutine* coro);
    std::function<void (float)> m_pushBallFunction;

};