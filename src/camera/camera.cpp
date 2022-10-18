
#include "camera/camera.h"
#include "debug.h"

glm::mat4 Camera::getViewMatrix()
{
    glm::quat reverseOrient = glm::conjugate(m_orientation);
    glm::mat4 rot = glm::toMat4(reverseOrient);

    glm::mat4 translation = glm::translate(glm::mat4(1.0), -m_position);

    return rot * translation;
}


void Camera::setAngles(float angleX, float angleY)
{
    m_angleRight = angleX;
    m_angleUp = angleY;

    updateCameraVectors();
}


void Camera::offsetAngles(float xoffset, float yoffset, bool constrainPitch)
{
    m_angleRight += xoffset;
    m_angleUp += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_angleUp > 90.0f)
            m_angleUp = 90.0f;
        if (m_angleUp < -90.0f)
            m_angleUp = -90.0f;
    }

    updateCameraVectors();
}


void Camera::updateCameraVectors()
{
    // quaternion black magic
    glm::quat aroundY = glm::angleAxis(glm::radians(-m_angleRight), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::quat aroundX = glm::angleAxis(glm::radians(m_angleUp), glm::vec3(1.0f, 0.0f, 0.0f));

    m_orientation = aroundY * aroundX;
    // rotate first on Yaxis, then rotate on Xaxis

    glm::quat qF = m_orientation * glm::quat(0.0f, 0.0f, 0.0f, -1.0f) * glm::conjugate(m_orientation);

    m_front = m_orientation * glm::vec3(0.0f, 0.0f, -1.0f); //{qF.x, qF.y, qF.z};

    m_right = m_orientation * glm::vec3(1.0f, 0.0f, 0.0f);
};

