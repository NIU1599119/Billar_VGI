#pragma once
#include "camera/camera_controller.h"

class CameraControllerOrbit : public CameraController {
public:
    CameraControllerOrbit(Camera* camera, float speed = 2.5f, float radius = 3.0f, glm::vec3* center = new glm::vec3(0.0f)) : CameraController(camera), m_speed(speed), m_radius(radius), m_center(center) {};

    void move(DIRECTION direction, float deltaTime)
    {
        switch (direction)
        {
        case UP:
            m_offset.y -= m_speed * deltaTime * 20;
            break;
        case DOWN:
            m_offset.y += m_speed * deltaTime * 20;
            break;
        case LEFT:
            m_offset.x += m_speed * deltaTime * 20;
            break;
        case RIGHT:
            m_offset.x -= m_speed * deltaTime * 20;
            break;
        default:
            break;
        }
    }

    void rotate(float rotX, float rotY)
    {
        m_camera->offsetAngles(rotX, rotY);

        m_offset = glm::vec2(rotX, rotY);
        updatePos();
    }

    void update()
    {
        m_camera->offsetAngles(m_offset.x, m_offset.y);

        updatePos();
    }

private:
    float m_speed;
    float m_radius;

    glm::vec3* m_center;

    glm::vec2 m_offset = glm::vec2(0.0f, 0.0f); // x is rotation on Up vector, y is rotation on Right vector

    void updatePos()
    {
        glm::vec3 pos = -m_camera->getFront() * m_radius + (*m_center);
        m_camera->setPosition(pos);

        m_offset = glm::vec2(0.0f, 0.0f);
    }

};

