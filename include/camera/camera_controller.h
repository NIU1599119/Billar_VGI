#pragma once

#include "camera/camera.h"
#include "input.h"
#include "debug.h"

class CameraController {
public:
    CameraController(Camera* camera) : m_camera(camera) {};

    enum DIRECTION {
        UP,
        DOWN,
        RIGHT,
        LEFT,
        // directions below act as aliases for the different implementations
        FRONT = UP,
        BACK = DOWN,
    };

    virtual void move(DIRECTION dir, float deltaTime) = 0;
    // move is supposed to handle multiple inputs and then save them to a variable

    virtual void rotate(float rotX, float rotY) = 0;
    // this rotates the camera if needed or sends an error if it should not be possible

    virtual void update() = 0;
    // update uses that variable and finally updates the camera

protected:
    Camera* m_camera;
};


inline void bindInputToController(Input* input, CameraController* controller)
{
    input->setActionFunction(MOVE_FORWARDS, [controller](float deltaTime){
        controller->move(controller->UP, deltaTime);
    });

    input->setActionFunction(MOVE_BACKWARDS, [controller](float deltaTime){
        controller->move(controller->DOWN, deltaTime);
    });
    
    input->setActionFunction(MOVE_LEFT, [controller](float deltaTime){
        controller->move(controller->LEFT, deltaTime);
    });
    
    input->setActionFunction(MOVE_RIGHT, [controller](float deltaTime){
        controller->move(controller->RIGHT, deltaTime);
    });

    input->setMouseCallback([controller](float xoffset, float yoffset){
        controller->rotate(xoffset, yoffset);
    });

    LOG_DEBUG("Correctly bound input to controller");

};

inline void unbindInputToController(Input* input)
{
    input->removeActionFunction(MOVE_FORWARDS);
    input->removeActionFunction(MOVE_BACKWARDS);
    input->removeActionFunction(MOVE_RIGHT);
    input->removeActionFunction(MOVE_LEFT);
    /// this will crash if an input is made before another bind happens
    /// after this unbind one should remove the keys from input
    input->setMouseCallback([](float xoffset, float yoffset) {});
}