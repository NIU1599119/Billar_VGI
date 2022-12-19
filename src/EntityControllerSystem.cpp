#include "EntityControllerSystem.h"


EntityControllerSystem::EntityControllerSystem(GAMEMODE gamemode, Rendering::RenderEngine3D* renderingEngine, std::vector<int>& ballRenderIDs, int maxSubSteps, double fixedTimeStep)
    : // simple initial constructors
    m_gamemode(gamemode),
    m_maxSubSteps(maxSubSteps),
    m_fixedTimeStep(fixedTimeStep),
    m_renderEngine(renderingEngine),
    m_ballRenderIDs(ballRenderIDs)
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    m_collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_dispatcher->setNearCallback((btNearCallback)CollisionDetectorNearCallback);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    m_overlappingPairCache = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    m_solver = new btSequentialImpulseConstraintSolver;

    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

    m_dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
    ///-----initialization_end-----

    //the ground is a cube of side 100 at position y = -56.
    //the sphere will hit it at y = -6, with center at -5
    std::vector<glm::vec3> wallPos;
    std::vector<glm::vec3> wallSizes;
    // 1 10 1

    /*    ------------ X ------------
        ┌──┬──────────┬─┬───────────┬─┐
        │O └──────────┘O└───────────┘O│ |
        ├┐                           ┌┤ |
        ││                           ││ |
        ││                           ││ |
        ││                           ││ Y
        ││                           ││ |
        ││                           ││ |
        ├┘                           └┤ |
        │O┌───────────┐O┌───────────┐O│ |
        └─┴───────────┴─┴───────────┴─┘
    */

    Rendering::CollisionBox table;
    table.setFriction(.8165);
    table.setRestitution(.513);

    Rendering::CollisionBox wallAux(table.getMesh());
    wallAux.setFriction(.8165);             // all walls have this values
    wallAux.setRestitution(.7695);          // restitucion real (se supone)

    ///////////// Inicialization EntityPool /////////////////

    int wallsIdStart = 0;

    switch(gamemode) 
    {
        case CLASSIC:
        case FREE_SHOTS:
        {
            table.setPosition(glm::vec3(0., 0.76/2.0, 0.));
            table.setScale(glm::vec3(2.24, 0.76, 1.12));

            m_EntityPool.push_back(table);

            wallsIdStart = m_EntityPool.size();
            
            // posicion es el limite del tablero menos la mitad del ancho
            // 262-224 = 150-112 = 38 cm Anchura paredes -> 19 anchura 1 pared
            // 81.3-76.5 = 4.8 cm Altura pared (comparado con el tablero)

            // wall (front 1)
            wallAux.setPosition(glm::vec3(0.55, 0.76, -(0.75-0.095)));
            wallAux.setScale(glm::vec3(1.00, 0.096, 0.19));
            m_EntityPool.push_back(wallAux);

            // wall (front 2)
            wallAux.setPosition(glm::vec3(-0.55, 0.76, -(0.75 - 0.095)));
            wallAux.setScale(glm::vec3(1.00, 0.096, 0.19));
            m_EntityPool.push_back(wallAux);

            // wall (back 1)
            wallAux.setPosition(glm::vec3(0.55, 0.76, +(0.75-0.095)));
            wallAux.setScale(glm::vec3(1.00, 0.096, 0.19));
            m_EntityPool.push_back(wallAux);

            // wall (back 2)
            wallAux.setPosition(glm::vec3(-0.55, 0.76, +(0.75 - 0.095)));
            wallAux.setScale(glm::vec3(1.00, 0.096, 0.19));
            m_EntityPool.push_back(wallAux);

            // wall (left)
            wallAux.setPosition(glm::vec3(-(1.31-0.095), 0.76, 0.));
            wallAux.setScale(glm::vec3(0.19, 0.096, 1.00));
            m_EntityPool.push_back(wallAux);

            // wall (right)
            wallAux.setPosition(glm::vec3(+(1.31-0.095), 0.76, 0.));
            wallAux.setScale(glm::vec3(0.19, 0.096, 1.00));
            m_EntityPool.push_back(wallAux);

            // wall hole 1 (left)
            wallAux.setPosition(glm::vec3(+(1.31 - (0.095+0.028)), 0.76, 0.535));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.15));
            wallAux.setAngle(glm::radians(45.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 1 (right)
            wallAux.setPosition(glm::vec3(+(1.31 - (0.095 + 0.12)), 0.76, 0.63));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.15));
            wallAux.setAngle(glm::radians(45.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 1 (back)
            wallAux.setPosition(glm::vec3(+(1.31 - (0.095 + 0.0)), 0.76, 0.63));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.20));
            wallAux.setAngle(glm::radians(135.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 2 (left)
            wallAux.setPosition(glm::vec3(-(1.31 - (0.095 + 0.028)), 0.76, 0.535));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.15));
            wallAux.setAngle(glm::radians(135.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 2 (right)
            wallAux.setPosition(glm::vec3(-(1.31 - (0.095 + 0.12)), 0.76, 0.63));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.15));
            wallAux.setAngle(glm::radians(135.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 2 (back)
            wallAux.setPosition(glm::vec3(-(1.31 - (0.095 + 0.0)), 0.76, 0.63));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.20));
            wallAux.setAngle(glm::radians(45.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 3 (left)
            wallAux.setPosition(glm::vec3(+(1.31 - (0.095 + 0.028)), 0.76, -0.535));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.15));
            wallAux.setAngle(glm::radians(135.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 3 (right)
            wallAux.setPosition(glm::vec3(+(1.31 - (0.095 + 0.12)), 0.76, -0.63));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.15));
            wallAux.setAngle(glm::radians(135.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 3 (back)
            wallAux.setPosition(glm::vec3(+(1.31 - (0.095 + 0.0)), 0.76, -0.63));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.20));
            wallAux.setAngle(glm::radians(45.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 4 (left)
            wallAux.setPosition(glm::vec3(-(1.31 - (0.095 + 0.028)), 0.76, -0.535));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.15));
            wallAux.setAngle(glm::radians(45.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 4 (right)
            wallAux.setPosition(glm::vec3(-(1.31 - (0.095 + 0.12)), 0.76, -0.63));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.15));
            wallAux.setAngle(glm::radians(45.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 4 (back)
            wallAux.setPosition(glm::vec3(-(1.31 - (0.095 + 0.0)), 0.76, -0.63));
            wallAux.setScale(glm::vec3(0.04, 0.096, 0.20));
            wallAux.setAngle(glm::radians(135.0f));
            m_EntityPool.push_back(wallAux);

            // wall hole 5
            wallAux.setPosition(glm::vec3(0, 0.76, +(0.75 - 0.080 / 2)));
            wallAux.setScale(glm::vec3(0.10, 0.096, 0.080));
            wallAux.setAngle(glm::radians(0.0f));
            m_EntityPool.push_back(wallAux);

            //wall hole 6
            wallAux.setPosition(glm::vec3(0, 0.76, -(0.75 - 0.080/2)));
            wallAux.setScale(glm::vec3(0.10, 0.096, 0.080));
            wallAux.setAngle(glm::radians(0.0f));
            m_EntityPool.push_back(wallAux);


            break;
        }
        case CARAMBOLA:
            table.setPosition(glm::vec3(0., 0.76 / 2.0, 0.));
            table.setScale(glm::vec3(2.62, 0.76, 1.50));

            m_EntityPool.push_back(table);
            wallsIdStart = m_EntityPool.size();

            // wall (front)
            wallAux.setPosition(glm::vec3(0., 0.76, -(0.75 - 0.095)));
            // posicion es el limite del tablero menos la mitad del ancho
            wallAux.setScale(glm::vec3(2.62, 0.096, 0.19));
            // 262-224 = 150-112 = 38 cm Anchura paredes -> 19 anchura 1 pared
            // 81.3-76.5 = 4.8 cm Altura pared (comparado con el tablero)

            m_EntityPool.push_back(wallAux);

            // wall (back)
            wallAux.setPosition(glm::vec3(0., 0.76, +(0.75 - 0.095)));
            wallAux.setScale(glm::vec3(2.62, 0.096, 0.19));

            m_EntityPool.push_back(wallAux);

            // wall (left)
            wallAux.setPosition(glm::vec3(-(1.31 - 0.095), 0.76, 0.));
            wallAux.setScale(glm::vec3(0.19, 0.096, 1.50));

            m_EntityPool.push_back(wallAux);

            // wall (right)
            wallAux.setPosition(glm::vec3(+(1.31 - 0.095), 0.76, 0.));
            wallAux.setScale(glm::vec3(0.19, 0.096, 1.50));

            m_EntityPool.push_back(wallAux);
            break;
        default:
            break;
    }

    for (int i = 0; i < m_EntityPool.size(); i++)
    {
        btRigidBody* body = m_EntityPool[i].getRigidBodyBullet(m_collisionShapes);
        
        Entities::Entity* tablePart;
        if (i < wallsIdStart)
            tablePart = new Entities::EntityTable(gamemode, Entities::CLOTH);
        else
            tablePart = new Entities::EntityTable(gamemode, Entities::RAIL);
        body->setUserPointer(tablePart);
        m_dynamicsWorld->addRigidBody(body);
        m_entities.push_back(tablePart);
    }

    /////////// Posiciones ///////////////

    m_ballsIndex = m_collisionShapes.size();

    std::vector<btVector3> entityBallPositions;


    switch (gamemode)
    {
    case CLASSIC:
        if (m_ballRenderIDs.size() != 16)
        {
            LOG_ERROR("Incorrect number of balls");
            break;
        }

        // Bola blanca
        entityBallPositions.push_back(btVector3(0, 3, 0));

        // Fila 1
        entityBallPositions.push_back(btVector3(0.7, 3, 0));

        // Fila 2
        entityBallPositions.push_back(btVector3(0.75, 3, 0.03));
        entityBallPositions.push_back(btVector3(0.75, 3, -0.03));

        // Fila 3
        entityBallPositions.push_back(btVector3(0.8, 3, 0));
        entityBallPositions.push_back(btVector3(0.8, 3, 0.06));
        entityBallPositions.push_back(btVector3(0.8, 3, -0.06));

        // Fila 4
        entityBallPositions.push_back(btVector3(0.85, 3, 0.09));
        entityBallPositions.push_back(btVector3(0.85, 3, 0.03));
        entityBallPositions.push_back(btVector3(0.85, 3, -0.03));
        entityBallPositions.push_back(btVector3(0.85, 3, -0.09));

        // Fila 5
        entityBallPositions.push_back(btVector3(0.9, 3, 0.12));
        entityBallPositions.push_back(btVector3(0.9, 3, 0.06));
        entityBallPositions.push_back(btVector3(0.9, 3, 0));
        entityBallPositions.push_back(btVector3(0.9, 3, -0.06));
        entityBallPositions.push_back(btVector3(0.9, 3, -0.12));
    
        break;
    
    case CARAMBOLA:
        break;
    case FREE_SHOTS:
        break;
    default:
        break;
    }


    for (int i = 0; i < entityBallPositions.size(); i++)
    {
        //create a dynamic rigidbody

        //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
        btCollisionShape* colShape = new btSphereShape(btScalar(0.05715 / 2.0));  // 0.05715 is diameter, function takes radius
        m_collisionShapes.push_back(colShape);

        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();

        btScalar mass(0.17f);   // 170 grams

        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (mass != 0.f);

        btVector3 localInertia(0, 0, 0);
        if (isDynamic)
            colShape->calculateLocalInertia(mass, localInertia);

        startTransform.setOrigin(entityBallPositions[i]);

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setFriction(.245);
        body->setRestitution(0.97468);
        body->setContactProcessingThreshold(0);// ESTE ERA EL THRESHOLD DIOOOOOOS
        body->setSpinningFriction(0.8);
        body->setRollingFriction(0.0004);


        Entities::Entity* ballEntity = new Entities::EntityBall(i, gamemode);
        body->setUserPointer(ballEntity);
        m_dynamicsWorld->addRigidBody(body);
        m_entities.push_back(ballEntity);
    }
}

EntityControllerSystem::~EntityControllerSystem()
{
    for (int i = 0; i < m_entities.size(); i++)
    {
        if (m_entities[i] != nullptr)
            delete m_entities[i];
    }

    if (m_dynamicsWorld != nullptr)
        delete m_dynamicsWorld;

    if (m_solver != nullptr)
        delete m_solver;

    if (m_overlappingPairCache != nullptr)
        delete m_overlappingPairCache;

    if (m_dispatcher != nullptr)
        delete m_dispatcher;

    if (m_collisionConfiguration != nullptr)
        delete m_collisionConfiguration;

}


void EntityControllerSystem::update(double deltaTime, glm::vec3* focusedBallPosition, int focusedBallID)
{
    m_dynamicsWorld->stepSimulation(deltaTime, m_maxSubSteps, m_fixedTimeStep);

    // update the rendering engine
    int i = 0;
    for (auto it = m_ballRenderIDs.begin(); it != m_ballRenderIDs.end(); it++)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[m_ballsIndex + i];   // should be ball
        btRigidBody* body = btRigidBody::upcast(obj);
        btTransform trans;

        if (body && body->getMotionState())
        {
            body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
            trans = obj->getWorldTransform();
        }

        // btVector3 pos = trans.getOrigin();
        glm::vec3 pos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
        if (i == 0)
            (*focusedBallPosition) = pos;   // update the camera 
        glm::quat orient = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ()); // probar de cambiar esto de wxyz a xyzw por si bullet los tiene en ordenes distintos

        
        m_renderEngine->updateObject(*it, pos, orient);

        i++;
    }
}


/// @brief Modified function from dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo)
/// @param collisionPair
/// @param dispatcher
/// @param dispatchInfo
void CollisionDetectorNearCallback(btBroadphasePair& collisionPair,
  btCollisionDispatcher& dispatcher, btDispatcherInfo& dispatchInfo) {

	btCollisionObject* colObj0 = (btCollisionObject*)collisionPair.m_pProxy0->m_clientObject;
	btCollisionObject* colObj1 = (btCollisionObject*)collisionPair.m_pProxy1->m_clientObject;

	if (!dispatcher.needsCollision(colObj0, colObj1))
    {
        return;
    }

    btCollisionObjectWrapper obj0Wrap(0, colObj0->getCollisionShape(), colObj0, colObj0->getWorldTransform(), -1, -1);
    btCollisionObjectWrapper obj1Wrap(0, colObj1->getCollisionShape(), colObj1, colObj1->getWorldTransform(), -1, -1);

    //dispatcher will keep algorithms persistent in the collision pair
    if (!collisionPair.m_algorithm)
    {
        collisionPair.m_algorithm = dispatcher.findAlgorithm(&obj0Wrap, &obj1Wrap, 0, BT_CONTACT_POINT_ALGORITHMS);
    }


    btManifoldResult contactPointResult(&obj0Wrap, &obj1Wrap);

    if (dispatchInfo.m_dispatchFunc == btDispatcherInfo::DISPATCH_DISCRETE)
    {
        //discrete collision detection query
        collisionPair.m_algorithm->processCollision(&obj0Wrap, &obj1Wrap, dispatchInfo, &contactPointResult);
    }
    else
    {
        //continuous collision detection query, time of impact (toi)
        btScalar toi = collisionPair.m_algorithm->calculateTimeOfImpact(colObj0, colObj1, dispatchInfo, &contactPointResult);
        if (dispatchInfo.m_timeOfImpact > toi)
            dispatchInfo.m_timeOfImpact = toi;
    }

    /// custom code a partir de aqui

    if (contactPointResult.getPersistentManifold()->getNumContacts() == 0)    // no ha habido colision
        return;

    
    const btRigidBody* body0 = btRigidBody::upcast(colObj0);
    const btRigidBody* body1 = btRigidBody::upcast(colObj1);

    if (body0 == nullptr || body1 == nullptr || body0->getMotionState() == nullptr || body1->getMotionState() == nullptr)   // check if the pointers are OK and if the objects are active (in movement)
    {
        return;
    }
    btVector3 bt_vel0 = body0->getLinearVelocity();
    btVector3 bt_vel1 = body1->getLinearVelocity();

    double deltaSpeed = (bt_vel0 - bt_vel1).length();

    if (deltaSpeed < 0.0001)     // si la colision es demasiado lenta salimos (significa que se estan tocando)
        return;

    Entities::Entity* entity0 = (Entities::Entity*) colObj0->getUserPointer();
    Entities::Entity* entity1 = (Entities::Entity*) colObj1->getUserPointer();

    if (entity0 == nullptr || entity1 == nullptr)
        return;
    
    entity0->collision(entity1, deltaSpeed); // let them handle it
}

bool EntityControllerSystem::isStatic()
{
    bool simulationIsStatic = true;

    btCollisionObjectArray currentObjectArray = m_dynamicsWorld->getCollisionObjectArray();
    for (int i = 0; i < currentObjectArray.size(); i++)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);

        int activationState = body->getActivationState();

        if (activationState != ACTIVE_TAG)
            continue;
        
        simulationIsStatic = false;
        break;
    }

    return simulationIsStatic;
}

int EntityControllerSystem::createEntity(Entities::Entity* newEntity, btCollisionShape* colShape, btVector3& position)
{
    //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    // btCollisionShape* colShape = new btSphereShape(btScalar(0.05715 / 2.0));  // 0.05715 is diameter, function takes radius
    int entityID = m_collisionShapes.size();
    m_collisionShapes.push_back(colShape);

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(0.);   // 170 grams

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass, localInertia);

    // startTransform.setOrigin();

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setUserPointer(newEntity);
    m_dynamicsWorld->addRigidBody(body);
    return entityID;
}