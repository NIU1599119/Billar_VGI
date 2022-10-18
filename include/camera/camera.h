#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "debug.h"

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f))
    {
        m_position = position;
        m_worldUp = worldUp;
        m_orientation = glm::quat(0, 0, 0, -1);

        m_angleRight = 0.0f;
        m_angleUp = 0.0f;
        // m_worldUp = worldUp;
        // m_yaw = yaw;
        // m_pitch = pitch;
        updateCameraVectors();
    };

    glm::mat4 getViewMatrix();

    void setPosition(glm::vec3 position) { m_position = position; };

    void offsetPosition(glm::vec3 offset) { m_position = m_position + offset; };

    void setAngles(float angleX, float angleY);

    void offsetAngles(float xoffset, float yoffset, bool constrainPitch = true);

    glm::vec3 getFront() { return m_front; };

    glm::vec3 getRight() { return m_right; };

    glm::vec3 getPlaneFront() { return -glm::normalize(glm::cross(m_right, m_worldUp)); };

private:
    glm::quat m_orientation;
    glm::vec3 m_position;

    float m_angleRight;
    float m_angleUp;

    glm::vec3 m_front;
    glm::vec3 m_right;

    glm::vec3 m_worldUp;

    void updateCameraVectors();
};

