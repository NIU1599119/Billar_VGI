#pragma once

#include "rendering/model.h"

namespace Rendering {

    class Object {
    public:
        Object(Model* model, glm::vec3 scale = glm::vec3(1.0f)) : m_model(model), m_modelScaling(scale) {};

        void draw(Shader* shader, glm::mat4& view, glm::mat4& projection, glm::vec3 cameraPosition);
        void draw(Shader* shader);

        void setPosition(glm::vec3& position) { m_position = position; };
        glm::vec3 getPosition() { return m_position; };

        void setOrientation(glm::quat& orientation) { m_orientation = glm::normalize(orientation); };    // usar esto si bullet tiene quaterniones
        void setOrientation(float radians, glm::vec3& direction) { m_orientation = glm::normalize(glm::angleAxis(radians, direction)); /* radians is the angle in radians */ };
        glm::quat getOrientation() { return m_orientation; };

        void setScaling(double scale) { m_scaling = glm::vec3(scale); };
        void setScaling(glm::vec3 scale) { m_scaling = scale; };
        glm::vec3 getScaling() { return m_scaling; };

        void rotate(float radians, glm::vec3& direction) { // angle is in radians
            glm::quat rotation = glm::angleAxis(radians, direction);
            m_orientation = glm::normalize(m_orientation*rotation);
        };

    private:
        Model* m_model;

        glm::vec3 m_position = glm::vec3(0.0f);
        glm::quat m_orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        glm::vec3 m_modelScaling = glm::vec3(1.0f);     // escalado del modelo para que tenga proporciones reales (especificado 1 vez)
        glm::vec3 m_scaling = glm::vec3(1.0f);          // escalado que le aplicamos si queremos (se actualiza con el tiempo)
    };
};