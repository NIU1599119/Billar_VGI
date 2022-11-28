#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "window.h"
#include "gl_utils.h"

#include "rendering/shader.h"
#include "rendering/texture.h"

#include "rendering/model.h"
#include "rendering/lightPoint.h"
#include "rendering/object.h"


#include "camera/camera.h"
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

    Shader modelShader("shaders/model.vert", "shaders/model.frag");
    if(!modelShader.compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        return 1;
    }

    Shader lightShader("shaders/light.vert", "shaders/light.frag");
    if(!lightShader.compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        return 1;
    }

    #ifdef DEBUG_SHADER
    Shader debugShader("shaders/debug.vert", "shaders/debug.frag");
    if(!debugShader.compileShaders())
    {
        LOG_ERROR("Failed compiling shader");
        return 1;
    }
    #endif


    /////////// IMGUI init ///////////

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool drawTriangles = true;

    glm::vec3 controlledPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 controlledDirection = glm::vec3(0.5f, 1.0f, 0.0f);

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

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // camera perspective to screen
    glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f); /// this should be updated when window changes size


    enum CameraType {
        FLY,
        FPS,
        ORBIT
    };
    CameraType currentType = ORBIT;

    CameraController* cameraController = new CameraControllerOrbit(&camera, 2.5f, 1.0f, &controlledPosition);
    bindInputToController(input, cameraController);


    // here goes the models
    Rendering::Model poolTableModel("models/pooltable/Pool table Cavicchi Leonardo 9FT N300818.3ds");
    Rendering::Model ball1Model("models/PoolBall1/Pool.obj");
    Rendering::Model ball8Model("models/PoolBall/Pool.obj");

    Rendering::Object poolTable(&poolTableModel);
    Rendering::Object ball1(&ball1Model);
    Rendering::Object ball8(&ball8Model);

    /////////// LIGHTS ///////////

    glm::vec3 lightCubePosition = glm::vec3( 0.0f,  1.2f,  0.0f);
    glm::vec3 lightCubeColor = glm::vec3(1.0f, 1.0f, 1.0f);

    LOG_DEBUG("Loading lights...");
    Rendering::LightPoint lightPoint(0);
    LOG_DEBUG("Loaded lights");

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
    for (int i = 0; i < 2; i++) // create 2 balls       // code should be at ball creation
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

		startTransform.setOrigin(btVector3(0+i, 3, 0));

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
    
    input->setKeyAction(PUSH_BALL, GLFW_KEY_F);
    input->setActionFunction(PUSH_BALL, [&dynamicsWorld, &ballsIndex](float deltaTime){
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex];
        btRigidBody* body = btRigidBody::upcast(obj);
        body->setActivationState(ACTIVE_TAG);
        body->setLinearVelocity(btVector3(1, body->getLinearVelocity().y(), 1));
    });


    input->setKeyAction(ACCELERATE_BALL, GLFW_KEY_E);
    input->setActionFunction(ACCELERATE_BALL, [&dynamicsWorld, &ballsIndex](float deltaTime){
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex];
        btRigidBody* body = btRigidBody::upcast(obj);
        body->setActivationState(ACTIVE_TAG);
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

        glm::mat4 view = camera.getViewMatrix();

        /////////// RENDERING ///////////
        if (drawTriangles)
        {
            lightPoint.setPosition(lightCubePosition);
            lightPoint.draw(&lightShader, view, projection);
            lightPoint.updateShader(&modelShader);

            glm::vec3 poolTablePosition = glm::vec3(0.0f);
            poolTable.setPosition(poolTablePosition);
            glm::vec3 poolTableOrientation = glm::vec3(0.0f, 1.0f, 0.0f);
            poolTable.setOrientation(0.0f, poolTableOrientation);
            poolTable.setScaling(0.001f);

            poolTable.draw(&modelShader, view, projection, camera.getPosition());

            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex];   // should be ball
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
            glm::quat orient = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ()); // probar de cambiar esto de wxyz a xyzw por si bullet los tiene en ordenes distintos

            controlledPosition = pos;

            ball8.setPosition(pos);
            ball8.setOrientation(orient);
            ball8.setScaling(0.05715f/2);

            ball8.draw(&modelShader, view, projection, camera.getPosition());

            obj = dynamicsWorld->getCollisionObjectArray()[ballsIndex+1];   // should be second ball
            body = btRigidBody::upcast(obj);
            // trans;
            if (body && body->getMotionState())
            {
                body->getMotionState()->getWorldTransform(trans);
            }
            else
            {
                trans = obj->getWorldTransform();
            }
            // btVector3 pos = trans.getOrigin();
            pos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
            orient = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());

            ball1.setPosition(pos);
            ball1.setOrientation(orient);
            ball1.setScaling(0.05715f/2);

            ball1.draw(&modelShader, view, projection, camera.getPosition());

            #ifdef DEBUG_SHADER
            for (int i = 0; i < rigidObjects.size(); i++)
            {
                rigidObjects[i].draw(&debugShader, view, projection);
            }
            #endif
        }


        /////////// ImGui ///////////

        ImGui::Begin("Debug Window");
        ImGui::Checkbox("Draw Triangles", &drawTriangles);
        ImGui::Separator();
        ImGui::Text("Cube Controls");
        ImGui::SliderFloat3("Position", glm::value_ptr(controlledPosition), -1.5f, 1.5f);
        ImGui::SliderFloat3("Direction", glm::value_ptr(controlledDirection), -1.0f, 1.0f);
        ImGui::Separator();
        ImGui::Text("Light Cube Controls");
        ImGui::SliderFloat3("Light Position", glm::value_ptr(lightCubePosition), -2.0f, 2.0f);
        ImGui::ColorEdit3("Light Color", glm::value_ptr(*lightPoint.getColor()));
        ImGui::Separator();
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
        if (ImGui::Button("Camera orbit"))
        {
            unbindInputToController(input);
            if (cameraController != nullptr)
                delete cameraController;
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 0.2f, &controlledPosition);
            bindInputToController(input, cameraController);
        }
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