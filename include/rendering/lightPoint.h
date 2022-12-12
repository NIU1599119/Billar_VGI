#pragma once

// general
#include <glm/glm.hpp>

// renderizado
#include "rendering/shader.h"

namespace Rendering {
    class LightPoint
    {
    public:
        LightPoint() {};

        void updateShader(int id, Shader* targetShader);

        void setPosition(glm::vec3 position) { m_position = position; };
        void setColor(glm::vec3 color) { m_color = color; };

        void setPolinomial(float constant, float linear, float cuadratic) { m_constant = constant; m_linear = linear; m_cuadratic = cuadratic; };

        glm::vec3 getPosition() { return m_position; };
        glm::vec3 getColor() { return m_color; };

    private:

        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        float m_constant = 1.0f;
        float m_linear = 0.09f;
        float m_cuadratic = 0.0032f;

        glm::vec3 m_ambient = glm::vec3(0.1f);
        float m_diffuse = 0.7f;
        float m_specular = 1.0f;

        glm::vec3 m_color = glm::vec3(1.0f);
    };
};