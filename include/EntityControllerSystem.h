#pragma once

// basic includes
#include <string>
#include <vector>
#include "debug.h"

// matrix operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// fisicas
#include "btBulletDynamicsCommon.h"
#include "rendering/collisionBox.h"

// Input
//#include "input.h"
#include "window.h"
#include "gl_utils.h"

// Camera
#include "camera/camera.h"
#include "camera/camera_controller_fly.h"
#include "camera/camera_controller_fps.h"
#include "camera/camera_controller_orbit.h"

// Objects
#include "rendering/engine.h"
#include "rendering/object.h"

enum GAMEMODE {
    CLASSIC,
    CARAMBOLA,
    FREE_SHOTS
};


class EntityControllerSystem {
public:
    // EntityControllerSystem(int mode, Input* input, Camera* camera);
    EntityControllerSystem(GAMEMODE mode, Input* input, Camera* camera, Rendering::RenderEngine3D* renderingEngine, std::vector<int>& ballRenderIDs, int maxSubSteps = 5, double fixedTimeStep = 0.0016);
    
    void StepSimulation(btScalar timeStep, int maxSubSteps, btScalar fixedTimeStep);
    void update(double deltaTime, glm::vec3* focusedBallPosition);

    // void draw(Shader* modelShader, Camera* camera, glm::mat4& projection, glm::vec3& controlledPosition, Shader* debugShader);

    // btDiscreteDynamicsWorld* getDynamicsWorld() { return m_dynamicsWorld;  }
    // int getBallsIndex() { return m_ballsIndex; }

    // temporal function
    std::vector<Rendering::CollisionBox>* getCollisionsBoxPtr() { return &m_EntityPool; }

private:
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;

    int m_maxSubSteps;
    double m_fixedTimeStep;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    std::vector<Rendering::CollisionBox> m_EntityPool;
    // std::vector<btVector3> m_EntityBallPositions;
    // std::vector<Rendering::Object> m_EntityBallObjects;
    // std::vector<Rendering::Model> m_EntityBallModels;
    int m_ballsIndex;

    Rendering::RenderEngine3D* m_renderEngine;
    std::vector<int> m_ballRenderIDs;
};
