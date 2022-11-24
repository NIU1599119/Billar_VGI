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
#include "rendering/flexibleMesh.h"

#include "rendering/simpleModel.h"

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

// funciones

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ((WindowData *)glfwGetWindowUserPointer(window))->input.updateCursor(xpos, ypos);
}

void processInput(GLFWwindow *window, Input* input, float deltaTime)
{
    std::vector<Input::eventKey>* ekeys = input->getEventKeys();
    for (int i = 0; i < ekeys->size(); i++)
    {
        if (glfwGetKey(window, (*ekeys)[i].key) == GLFW_RELEASE && (*ekeys)[i].isPressed)
        {
            input->pressKey((*ekeys)[i].key, deltaTime);
            (*ekeys)[i].isPressed = false;
        }
        else if (glfwGetKey(window, (*ekeys)[i].key) == GLFW_PRESS)
            (*ekeys)[i].isPressed = true;
    }

    std::vector<int>* keys = input->getPollingKeys();
    for (int i = 0; i < keys->size(); i++)
    {
        if (glfwGetKey(window, (*keys)[i]) == GLFW_PRESS)
            input->pressKey((*keys)[i], deltaTime);
    }
}


// shader files
std::string vertexDir = "shaders/default.vert";
std::string fragmentDir = "shaders/default.frag";

// std::string textureDir = "textures/container2.png";


int main()
{

    Window window(1200, 900, "Billar", true);

    if (!window.initWindow()) {
        return 1;
    }

    // enables z-buffer
    glEnable(GL_DEPTH_TEST);


    /////////// LIGHT CUBE ///////////

    float lightCubeVertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    std::vector<AttributeData> lightCubeAttributes;
    lightCubeAttributes.push_back(AttributeData(3, 0));
    Mesh lightCubeMesh;
    lightCubeMesh.setVertex(lightCubeVertices, 36, 3*sizeof(float), lightCubeAttributes);
    lightCubeMesh.create();

    RenderingTemp::SimpleModel lightCube(&lightCubeMesh);
    glm::vec3 lightCubePosition = glm::vec3( 0.7f,  1.2f,  2.0f);
    lightCube.scale(0.2f);
    lightCube.setPosition(&lightCubePosition);
    glm::vec3 lightCubeColor = glm::vec3(1.0f, 1.0f, 1.0f);


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


    /////////// IMGUI init ///////////

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");


    bool drawTriangles = true;

    glm::vec3 controlledPosition = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 controlledDirection = glm::vec3(0.5f, 1.0f, 0.0f);


    /////////// INPUT init ///////////

    glfwSetCursorPosCallback(window.getGLFWwindow(), mouse_callback);

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
    glm::mat4 projection = glm::perspective(45.0f, 1200.0f / 800.0f, 0.1f, 100.0f); /// this should be updated when window changes size


    enum CameraType {
        FLY,
        FPS,
        ORBIT
    };
    CameraType currentType = ORBIT;

    CameraController* cameraController = new CameraControllerOrbit(&camera, 2.5f, 3.0f, &controlledPosition);
    bindInputToController(input, cameraController);


    // here goes the models
    Rendering::Model poolTableModel("models/pooltable/Pool table Cavicchi Leonardo 9FT N300818.3ds");
    Rendering::Model ball9Model("models/PoolBall/Pool.obj");

    Rendering::Object poolTable(&poolTableModel);
    Rendering::Object ball9(&ball9Model);
    Rendering::Object ballS(&ball9Model);

    // here goes the lights
    LOG_DEBUG("Loading lights...");
    Rendering::LightPoint lightPoint(0, true);
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
    std::vector<btVector3> wallPos;
    std::vector<btVector3> wallSizes;
    // 1 10 1
    wallPos.push_back(btVector3(0, 0, 0));
    wallSizes.push_back(btVector3(btScalar(10.), btScalar(1.), btScalar(10.)));
    wallPos.push_back(btVector3(btScalar(3.5), btScalar(1.), btScalar(0.)));
    wallSizes.push_back(btVector3(btScalar(1.), btScalar(2.), btScalar(10.)));

    for (int i = 0; i < 2; i++)
	{
		btCollisionShape* groundShape = new btBoxShape(wallSizes[i]);

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(wallPos[i]);

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}


    for (int i = 0; i < 2; i++) // create 2 balls       // code should be at ball creation
	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(0.057));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(0.5f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(1+i, 10, 1));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}

    input->setKeyAction(PUSH_BALL, GLFW_KEY_E);
    input->setActionFunction(PUSH_BALL, [&dynamicsWorld](float deltaTime){
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[2];   // 1 should be ball
        btRigidBody* body = btRigidBody::upcast(obj);
        body->setActivationState(ACTIVE_TAG);
        body->setLinearVelocity(btVector3(11.0, body->getLinearVelocity().y(), 0.0));
    });


    unsigned int nFrame = 0;
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while(!window.shouldClose())
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
            processInput(window.getGLFWwindow(), input, deltaTime);
        }
        // physics update outside (also update the position of everything)
        cameraController->update();
        dynamicsWorld->stepSimulation(deltaTime, 10);

        // //print positions of all objects
		// for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
		// {
		// 	btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
		// 	btRigidBody* body = btRigidBody::upcast(obj);
		// 	btTransform trans;
		// 	if (body && body->getMotionState())
		// 	{
		// 		body->getMotionState()->getWorldTransform(trans);
		// 	}
		// 	else
		// 	{
		// 		trans = obj->getWorldTransform();
		// 	}
		// 	printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		// }

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

            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[2];   // 2 should be ball
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
            glm::quat orient = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());

            ball9.setPosition(pos);
            ball9.setOrientation(orient);
            ball9.setScaling(0.057f);

            ball9.draw(&modelShader, view, projection, camera.getPosition());

            obj = dynamicsWorld->getCollisionObjectArray()[3];   // 3 should be second ball
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

            ballS.setPosition(pos);
            ballS.setOrientation(orient);
            ballS.setScaling(0.057f);

            ballS.draw(&modelShader, view, projection, camera.getPosition());
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
            cameraController = new CameraControllerOrbit(&camera, 2.5f, 3.0f, &controlledPosition);
            bindInputToController(input, cameraController);
        }
        ImGui::Text("Frame number %u", nFrame);
        ImGui::Text("FPS: %f", 1/deltaTime);
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