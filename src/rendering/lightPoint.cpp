#include "rendering/lightPoint.h"
#include "debug.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Rendering {

    void LightPoint::updateShader(int id, Shader* shader)
    {
        shader->activate();
        shader->setUniformVec3("pointLights[" + std::to_string(id) + "].position", m_position);
        shader->setUniformFloat("pointLights[" + std::to_string(id) + "].constant", m_constant);
        shader->setUniformFloat("pointLights[" + std::to_string(id) + "].linear", m_linear);
        shader->setUniformFloat("pointLights[" + std::to_string(id) + "].cuadratic", m_cuadratic);

        shader->setUniformVec3("pointLights[" + std::to_string(id) + "].ambient", m_ambient);
        shader->setUniformVec3("pointLights[" + std::to_string(id) + "].diffuse", m_color * m_diffuse);
        shader->setUniformVec3("pointLights[" + std::to_string(id) + "].specular", m_color * m_specular);
    }
}