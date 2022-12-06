#include "EntityControllerSystem.h"

EntityControllerSystem::EntityControllerSystem(int mode, Input* input, Camera* camera)
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    m_collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

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

    // wallPos.push_back(btVector3(btScalar(0.), btScalar(0.76/2), btScalar(0.)));   // table
    // wallSizes.push_back(btVector3(btScalar(2.62), btScalar(0.76), btScalar(1.50)));
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

    switch(mode) 
    {
        case CLASSIC:
            table.setPosition(glm::vec3(0., 0.76 / 2.0, 0.));
            table.setScale(glm::vec3(2.62, 0.76, 1.50));

            m_EntityPool.push_back(table);

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

            for (int i = 0; i < m_EntityPool.size(); i++)
            {
                m_EntityPool[i].initializeBullet(m_collisionShapes, m_dynamicsWorld);
            }

            break;

        case CARAMBOLA:
            break;

        case FREE_SHOTS:
            break;

        default:
            break;
    }

    //////////// Inicialization EntityBall ///////////////////
    
    //////////// Objetos ///////////////////
    Rendering::Model ball1Model("models/PoolBall1/Pool.obj");
    Rendering::Model ball8Model("models/PoolBall/Pool.obj");

    m_EntityBallModels.push_back(ball1Model);
    m_EntityBallModels.push_back(ball8Model);

    Rendering::Object ball(&m_EntityBallModels[0]);
    Rendering::Object ball1(&m_EntityBallModels[1]);
    Rendering::Object ball2(&m_EntityBallModels[1]);
    Rendering::Object ball3(&m_EntityBallModels[1]);
    Rendering::Object ball4(&m_EntityBallModels[1]);
    Rendering::Object ball5(&m_EntityBallModels[1]);
    Rendering::Object ball6(&m_EntityBallModels[1]);
    Rendering::Object ball7(&m_EntityBallModels[1]);
    Rendering::Object ball8(&m_EntityBallModels[1]);
    Rendering::Object ball9(&m_EntityBallModels[1]);
    Rendering::Object ball10(&m_EntityBallModels[1]);
    Rendering::Object ball11(&m_EntityBallModels[1]);
    Rendering::Object ball12(&m_EntityBallModels[1]);
    Rendering::Object ball13(&m_EntityBallModels[1]);
    Rendering::Object ball14(&m_EntityBallModels[1]);
    Rendering::Object ball15(&m_EntityBallModels[1]);

    switch (mode) 
    {
        case CLASSIC:
            m_EntityBallObjects.push_back(ball);
            m_EntityBallObjects.push_back(ball1);
            m_EntityBallObjects.push_back(ball2);
            m_EntityBallObjects.push_back(ball3);
            m_EntityBallObjects.push_back(ball4);
            m_EntityBallObjects.push_back(ball5);
            m_EntityBallObjects.push_back(ball6);
            m_EntityBallObjects.push_back(ball7);
            m_EntityBallObjects.push_back(ball8);
            m_EntityBallObjects.push_back(ball9);
            m_EntityBallObjects.push_back(ball10);
            m_EntityBallObjects.push_back(ball11);
            m_EntityBallObjects.push_back(ball12);
            m_EntityBallObjects.push_back(ball13);
            m_EntityBallObjects.push_back(ball14);
            m_EntityBallObjects.push_back(ball15);
            break;

        case CARAMBOLA:
            break;
        case FREE_SHOTS:
            break;
        default:
            break;
    }  

    /////////// Posiciones ///////////////

    m_ballsIndex = m_collisionShapes.size();

    switch (mode) 
    {
        case CLASSIC:
            // Bola blanca
            m_EntityBallPositions.push_back(btVector3(0, 3, 0));

            // Fila 1
            m_EntityBallPositions.push_back(btVector3(0.7, 3, 0));

            // Fila 2
            m_EntityBallPositions.push_back(btVector3(0.75, 3, 0.03));
            m_EntityBallPositions.push_back(btVector3(0.75, 3, -0.03));

            // Fila 3
            m_EntityBallPositions.push_back(btVector3(0.8, 3, 0));
            m_EntityBallPositions.push_back(btVector3(0.8, 3, 0.06));
            m_EntityBallPositions.push_back(btVector3(0.8, 3, -0.06));

            // Fila 4
            m_EntityBallPositions.push_back(btVector3(0.85, 3, 0.09));
            m_EntityBallPositions.push_back(btVector3(0.85, 3, 0.03));
            m_EntityBallPositions.push_back(btVector3(0.85, 3, -0.03));
            m_EntityBallPositions.push_back(btVector3(0.85, 3, -0.09));

            // Fila 5
            m_EntityBallPositions.push_back(btVector3(0.9, 3, 0.12));
            m_EntityBallPositions.push_back(btVector3(0.9, 3, 0.06));
            m_EntityBallPositions.push_back(btVector3(0.9, 3, 0));
            m_EntityBallPositions.push_back(btVector3(0.9, 3, -0.06));
            m_EntityBallPositions.push_back(btVector3(0.9, 3, -0.12));
            break;

        case CARAMBOLA:
            break;
        case FREE_SHOTS:
            break;
        default:
            break;
    }

    for (int i = 0; i < m_EntityBallPositions.size(); i++)
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

        startTransform.setOrigin(m_EntityBallPositions[i]);

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setFriction(.245);
        body->setRestitution(0.97468);
        body->setContactProcessingThreshold(0);// ESTE ERA EL THRESHOLD DIOOOOOOS
        body->setSpinningFriction(1);
        body->setRollingFriction(0.0005);
        m_dynamicsWorld->addRigidBody(body);
    }

}

void EntityControllerSystem::draw(Shader* modelShader, Camera* camera, glm::mat4& projection, glm::vec3& controlledPosition, Shader* debugShader)
{
    /// controlledPosition = pos; // Cosas del imgui que ya no sirven
    int i = 0;
    for (auto it = m_EntityBallObjects.begin(); it != m_EntityBallObjects.end(); it++)
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
            controlledPosition = pos;
        glm::quat orient = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ()); // probar de cambiar esto de wxyz a xyzw por si bullet los tiene en ordenes distintos

        
        it->setPosition(pos);
        it->setOrientation(orient);
        it->setScaling(0.05715f / 2);
        it->draw(modelShader, camera->getViewMatrix(), projection, camera->getPosition());
        i++;
    }

    #ifdef DEBUG_SHADER
        for (int i = 0; i < m_EntityPool.size(); i++)
        {
            m_EntityPool[i].draw(debugShader, camera->getViewMatrix(), projection);
        }
    #endif
}

