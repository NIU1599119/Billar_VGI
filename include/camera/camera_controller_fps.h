#pragma once
#include "camera/camera_controller.h"

class CameraControllerFps : public CameraController {
public:
    CameraControllerFps(Camera* camera, float speed = 2.5f) : CameraController(camera), m_speed(speed) {};

    void move(DIRECTION direction, float deltaTime)
    {
        switch (direction)
        {
        case FRONT:
            m_offset += m_speed * deltaTime * m_camera->getPlaneFront(); //getPlaneVector(m_camera->getFront());
            break;
        case BACK:
            m_offset -= m_speed * deltaTime * m_camera->getPlaneFront();
            break;
        case LEFT:
            m_offset -= m_speed * deltaTime * m_camera->getRight();
            break;
        case RIGHT:
            m_offset += m_speed * deltaTime * m_camera->getRight();
            break;
        default:
            break;
        }
    }

    void rotate(float rotX, float rotY)
    {
        m_camera->offsetAngles(rotX, rotY);
    }

    void update()
    {
        m_camera->offsetPosition(m_offset);
        m_offset = glm::vec3(0.0f, 0.0f, 0.0f);
    }

private:
    float m_speed;

    glm::vec3 m_offset = glm::vec3(0.0f, 0.0f, 0.0f);
};

