#pragma once

// Controllers
#include "EntityControllerSystem.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <iostream>
#include <math.h>

/*
#include "window.h"
#include "gl_utils.h"
*/

#include "rendering/shader.h"
#include "rendering/texture.h"

#include "rendering/model.h"
#include "rendering/lightPoint.h"
//#include "rendering/object.h"

/*
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
*/


int Game(Window& window);