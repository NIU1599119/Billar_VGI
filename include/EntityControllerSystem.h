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
    EntityControllerSystem(GAMEMODE mode, Input* input, Camera* camera, Rendering::RenderEngine3D* renderingEngine, std::vector<int>& ballRenderIDs);
    
    void StepSimulation(btScalar timeStep, int maxSubSteps, btScalar fixedTimeStep);

    // void draw(Shader* modelShader, Camera* camera, glm::mat4& projection, glm::vec3& controlledPosition, Shader* debugShader);

    btDiscreteDynamicsWorld* getDynamicsWorld() { return m_dynamicsWorld;  }
    // int getBallsIndex() { return m_ballsIndex; }

private:
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    std::vector<Rendering::CollisionBox> m_EntityPool;
    std::vector<btVector3> m_EntityBallPositions;
    // std::vector<Rendering::Object> m_EntityBallObjects;
    // std::vector<Rendering::Model> m_EntityBallModels;
    // int m_ballsIndex;

    Rendering::RenderEngine3D* m_renderEngine;
    std::vector<int> m_ballRenderIDs;
};
