#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "window.h"
#include "gl_utils.h"

#include "rendering/shader.h"
//#include "rendering/texture.h"

//#include "rendering/model.h"
//#include "rendering/lightPoint.h"
//#include "rendering/object.h"

#include "rendering/engine.h"


//#include "camera/camera.h"
// #include "input.h" // included in window
#include "camera/camera_controller_fly.h"
#include "camera/camera_controller_fps.h"
#include "camera/camera_controller_orbit.h"


// matrix operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// fisicas
#include "btBulletDynamicsCommon.h"
#include "rendering/collisionBox.h"


int Game(Window& window) {
    
    // enables z-buffer
    glEnable(GL_DEPTH_TEST);

    ///// loading shaders /////

    Rendering::Shader modelShader("shaders/model.vert", "shaders/model.frag");
    if(!modelShader.compileShaders())
    {
        LOG_ERROR("Failed compiling modelShader");
        return 1;
    }

    Rendering::Shader lightShader("shaders/light.vert", "shaders/light.frag");
    if(!lightShader.compileShaders())
    {
        LOG_ERROR("Failed compiling lightShader");
        return 1;
    }

    #ifdef DEBUG_SHADER
    Rendering::Shader debugShader("shaders/debug.vert", "shaders/debug.frag");
    if(!debugShader.compileShaders())
    {
        LOG_ERROR("Failed compiling debugShader");
        return 1;
    }
    #endif

    ///// rendering /////
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    Rendering::RenderEngine3D renderEngine(&camera, modelShader, &lightShader, &debugShader);
    int poolRenderID = renderEngine.createObject(std::string("models/pool_table/scene.gltf"), 0.1245);
    renderEngine.updateObject(poolRenderID, glm::vec3(0.0), glm::quat(1.0, 0.0, 0.0, 0.0));
    int blackBallRenderID = renderEngine.createObject(std::string("models/PoolBall/Pool.obj"), 0.05715 / 2);
    renderEngine.updateObject(blackBallRenderID, glm::vec3(0.0, 0.8, 0.1), glm::quat(1.0, 0.0, 0.0, 0.0));
    std::vector<int> ballsRenderIDs;
    ballsRenderIDs.push_back(blackBallRenderID);

    Rendering::Model ballModel("models/PoolBall1/Pool.obj");
    for (int i = 0; i < 15; i++) // create 15 balls
    {
        int idx = renderEngine.createObject(&ballModel, 0.05715 / 2);
        ballsRenderIDs.push_back(idx);
        renderEngine.updateObject(idx, glm::vec3(0.0, 0.8, 0.1), glm::quat(1.0, 0.0, 0.0, 0.0));
    }

    std::vector<int> lightsRenderID;
    for (int i = 0; i < 5; i++)
    {
        int idx = renderEngine.addLight();
        lightsRenderID.push_back(idx);
        renderEngine.setLightPosition(idx, glm::vec3(-1.0 + (i / 2.0), 2.5, 0.0));
        renderEngine.setLightColor(idx, glm::vec3(1.0, 1.0, 1.0));
        renderEngine.setLightPolinomial(idx, 1.0, 0.09, 0.0032);
    }

    /////////// IMGUI init ///////////

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool drawTriangles = true;

    //glm::vec3 controlledPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    //glm::vec3 controlledDirection = glm::vec3(0.5f, 1.0f, 0.0f);

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

    //Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // camera perspective to screen
    //glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f); /// this should be updated when window changes size


    enum CameraType {
        FLY,
        FPS,
        ORBIT
    };
    CameraType currentType = ORBIT;

    glm::vec3 cameraOrbitPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    CameraController* cameraController = new CameraControllerOrbit(&camera, 2.5f, 1.0f, &cameraOrbitPosition);
    bindInputToController(input, cameraController);

    //////////// FISICAS ////////////
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    

    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
	///-----initialization_end-----

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

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

    std::vector<Rendering::CollisionBox> rigidObjects;

    Rendering::CollisionBox table;
    table.setFriction(.8165);
    table.setRestitution(.513);

    table.setPosition(glm::vec3(0., 0.76/2.0, 0.));
    table.setScale(glm::vec3(2.62, 0.76, 1.50));

    rigidObjects.push_back(table);

    Rendering::CollisionBox wallAux(table.getMesh());
    wallAux.setFriction(.8165);             // all walls have this values
    wallAux.setRestitution(.7695);          // restitucion real (se supone)
    // wallAux.setRestitution(1.0);
    
    // wall (front)
    wallAux.setPosition(glm::vec3(0., 0.76, -(0.75-0.095)));
    // posicion es el limite del tablero menos la mitad del ancho
    wallAux.setScale(glm::vec3(2.62, 0.096, 0.19));
    // 262-224 = 150-112 = 38 cm Anchura paredes -> 19 anchura 1 pared
    // 81.3-76.5 = 4.8 cm Altura pared (comparado con el tablero)

    rigidObjects.push_back(wallAux);

    // wall (back)
    wallAux.setPosition(glm::vec3(0., 0.76, +(0.75-0.095)));
    wallAux.setScale(glm::vec3(2.62, 0.096, 0.19));

    rigidObjects.push_back(wallAux);

    // wall (left)
    wallAux.setPosition(glm::vec3(-(1.31-0.095), 0.76, 0.));
    wallAux.setScale(glm::vec3(0.19, 0.096, 1.50));

    rigidObjects.push_back(wallAux);

    // wall (right)
    wallAux.setPosition(glm::vec3(+(1.31-0.095), 0.76, 0.));
    wallAux.setScale(glm::vec3(0.19, 0.096, 1.50));

    rigidObjects.push_back(wallAux);

    for (int i = 0; i < rigidObjects.size(); i++)
    {
        rigidObjects[i].initializeBullet(collisionShapes, dynamicsWorld);
    }


    int ballsIndex = collisionShapes.size();

    std::vector<btVector3> vectorPosicions;

    // Bola blanca
    vectorPosicions.push_back(btVector3(0, 3, 0));

    // Fila 1
    vectorPosicions.push_back(btVector3(0.7, 3, 0));

    // Fila 2
    vectorPosicions.push_back(btVector3(0.75, 3, 0.03));
    vectorPosicions.push_back(btVector3(0.75, 3, -0.03));

    // Fila 3
    vectorPosicions.push_back(btVector3(0.8, 3, 0));
    vectorPosicions.push_back(btVector3(0.8, 3, 0.06));
    vectorPosicions.push_back(btVector3(0.8, 3, -0.06));

    // Fila 4
    vectorPosicions.push_back(btVector3(0.85, 3, 0.09));
    vectorPosicions.push_back(btVector3(0.85, 3, 0.03));
    vectorPosicions.push_back(btVector3(0.85, 3, -0.03));
    vectorPosicions.push_back(btVector3(0.85, 3, -0.09));

    // Fila 5
    vectorPosicions.push_back(btVector3(0.9, 3, 0.12));
    vectorPosicions.push_back(btVector3(0.9, 3, 0.06));
    vectorPosicions.push_back(btVector3(0.9, 3, 0));
    vectorPosicions.push_back(btVector3(0.9, 3, -0.06));
    vectorPosicions.push_back(btVector3(0.9, 3, -0.12));

    for (int i = 0; i < 16; i++) // create 16 balls       // code should be at ball creation
	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(0.05715/2.0));  // 0.05715 is diameter, function takes radius
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(0.17f);   // 170 grams

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(vectorPosicions[i]);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
    	body->setFriction(.245);
        body->setRestitution(0.97468);
        body->setContactProcessingThreshold(0);// ESTE ERA EL THRESHOLD DIOOOOOOS
        body->setSpinningFriction(1);
        body->setRollingFriction(0.0001);
		dynamicsWorld->addRigidBody(body);
	}

    std::vector<btCollisionObject*> ballsPhisics;
    for (int i = 0; i < 16; i++)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex+i];
        ballsPhisics.push_back(obj);
    }


    input->setKeyAction(PUSH_BALL, GLFW_KEY_F);
    input->setActionFunction(PUSH_BALL, [&dynamicsWorld, &camera, &ballsIndex](float deltaTime){
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex];
        btRigidBody* body = btRigidBody::upcast(obj);
        body->setActivationState(ACTIVE_TAG);
        glm::vec3 front = camera.getPlaneFront();
        front = front * 4.0f;
        body->setLinearVelocity(btVector3(front.x, body->getLinearVelocity().y(), front.z));
    });


    input->setKeyAction(ACCELERATE_BALL, GLFW_KEY_E);
    input->setActionFunction(ACCELERATE_BALL, [&dynamicsWorld, &camera, &ballsIndex](float deltaTime){
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex];
        btRigidBody* body = btRigidBody::upcast(obj);
        body->setActivationState(ACTIVE_TAG);
        glm::vec3 front = camera.getPlaneFront();
        front = front * 4.0f;
        body->setLinearVelocity(btVector3(body->getLinearVelocity().x()*(1.01+deltaTime), body->getLinearVelocity().y(), body->getLinearVelocity().z()*(1.01+deltaTime)));
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
        dynamicsWorld->stepSimulation(deltaTime, 100, 0.001f);

        //glm::mat4 view = camera.getViewMatrix();

        /////////// RENDERING ///////////
        if (drawTriangles)
        {
            for (int i = 0; i < 16; i++)
            {
                btCollisionObject* obj = ballsPhisics[i];
                btRigidBody* body = btRigidBody::upcast(obj);
                btTransform trans;
                if (body && body->getMotionState())
                    body->getMotionState()->getWorldTransform(trans);
                else
                    trans = obj->getWorldTransform();
                glm::vec3 pos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
                glm::quat orient = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());

                if (i == 0)
                    cameraOrbitPosition = pos;

                renderEngine.updateObject(ballsRenderIDs[i], pos, orient);
            }

            renderEngine.updateShaderView();
            renderEngine.drawAll();

            renderEngine.drawLights();

            #ifdef DEBUG_SHADER
            for (int i = 0; i < rigidObjects.size(); i++)
            {
                //rigidObjects[i].draw(&debugShader, view, projection);
                rigidObjects[i].draw(renderEngine.getDebuggingShader());
            }
            #endif
        }


        /////////// ImGui ///////////

        ImGui::Begin("Debug Window");
        ImGui::Checkbox("Draw Triangles", &drawTriangles);
        ImGui::Separator();
        //ImGui::Text("Cube Controls");
        //ImGui::SliderFloat3("Position", glm::value_ptr(controlledPosition), -1.5f, 1.5f);
        //ImGui::SliderFloat3("Direction", glm::value_ptr(controlledDirection), -1.0f, 1.0f);
        //ImGui::Separator();
        //ImGui::Text("Light Cube Controls");
        //ImGui::SliderFloat3("Light Position", glm::value_ptr(lightCubePosition), -2.0f, 2.0f);
        //ImGui::ColorEdit3("Light Color", glm::value_ptr(*lightPoint.getColor()));
        //ImGui::Separator();
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
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 1.0f, &cameraOrbitPosition);
            bindInputToController(input, cameraController);
        }
        if (ImGui::Button("Camera orbit (close)"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 0.2f, &cameraOrbitPosition);
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