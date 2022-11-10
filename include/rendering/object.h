#pragma once

#include "rendering/model.h"

namespace Rendering {

    class Object {
    public:
        Object(Model* model) : m_model(model) {};

        void draw(Shader* shader, glm::mat4& view, glm::mat4& projection, glm::vec3 cameraPosition);

        void setPosition(glm::vec3& position) { m_position = position; };

        void setOrientation(glm::quat& orientation) { m_orientation = glm::normalize(orientation); };    // usar esto si bullet tiene quaterniones
        void setOrientation(float angle, glm::vec3& direction) { m_orientation = glm::normalize(glm::angleAxis(angle, direction)); /* angle is in radians */ };

        void setScaling(float scale) { m_scaling = glm::vec3(scale); };

        void rotate(float angle, glm::vec3& direction) { // angle is in radians
            glm::quat rotation = glm::angleAxis(angle, direction);
            m_orientation = glm::normalize(m_orientation*rotation);
        };

        void setLighting();

    private:
        Model* m_model;

        glm::vec3 m_position;
        glm::quat m_orientation;

        glm::vec3 m_scaling = glm::vec3(1.0f);
    };
};