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
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>

#include <btBulletDynamicsCommon.h>
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

#include "game/gamemodes.h"
#include "entities/Entity.h"
#include "entities/EntityBall.h"
#include "entities/EntityTable.h"


class EntityControllerSystem {
public:
    // EntityControllerSystem(int mode, Input* input, Camera* camera);
    EntityControllerSystem(GAMEMODE mode, Rendering::RenderEngine3D* renderingEngine, std::vector<int>& ballRenderIDs, int maxSubSteps = 10, double fixedTimeStep = 0.0016);
    ~EntityControllerSystem();
    
    void StepSimulation(btScalar timeStep, int maxSubSteps, btScalar fixedTimeStep);
    void update(double deltaTime, glm::vec3* focusedBallPosition, int focusedBallID = 0);

    // void draw(Shader* modelShader, Camera* camera, glm::mat4& projection, glm::vec3& controlledPosition, Shader* debugShader);

    // btDiscreteDynamicsWorld* getDynamicsWorld() { return m_dynamicsWorld;  }
    // int getBallsIndex() { return m_ballsIndex; }

    // temporal function
    std::vector<Rendering::CollisionBox>* getCollisionsBoxPtr() { return &m_EntityPool; }

    // check if all balls are static (inactive)
    bool isStatic();

    btDiscreteDynamicsWorldMt** getDynamicsWorld() { return &m_dynamicsWorld; };
    int getBallIdx(int ballID = 0) { return m_ballsIndex+ballID; }; 

    int createEntity(Entities::Entity* newEntity, btCollisionShape* colShape, btVector3& position);

private:
    GAMEMODE m_gamemode;
    //btDefaultCollisionConfiguration* m_collisionConfiguration;
    //btCollisionDispatcher* m_dispatcher;
    //btBroadphaseInterface* m_overlappingPairCache;
    //btSequentialImpulseConstraintSolver* m_solver;
    //btDiscreteDynamicsWorld* m_dynamicsWorld;

    btITaskScheduler* m_taskScheduler;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcherMt* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btConstraintSolverPoolMt* m_solverPool;
    btDiscreteDynamicsWorldMt* m_dynamicsWorld;

    int m_maxSubSteps;
    double m_fixedTimeStep;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    std::vector<Rendering::CollisionBox> m_EntityPool;
    std::vector<Entities::Entity*> m_entities;

    int m_ballsIndex;

    Rendering::RenderEngine3D* m_renderEngine;
    std::vector<int> m_ballRenderIDs;
};

void CollisionDetectorNearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, btDispatcherInfo& dispatchInfo);