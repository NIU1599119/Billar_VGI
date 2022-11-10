#include "rendering/lightPoint.h"
#include "debug.h"


namespace Rendering {

    void LightPoint::updateShader(Shader* shader)
    {
        shader->activate();
        shader->setUniformVec3("pointLights[" + std::to_string(m_lightID) + "].position", m_position);
        shader->setUniformFloat("pointLights[" + std::to_string(m_lightID) + "].constant", m_constant);
        shader->setUniformFloat("pointLights[" + std::to_string(m_lightID) + "].linear", m_linear);
        shader->setUniformFloat("pointLights[" + std::to_string(m_lightID) + "].cuadratic", m_cuadratic);

        shader->setUniformVec3("pointLights[" + std::to_string(m_lightID) + "].ambient", m_ambient);
        shader->setUniformVec3("pointLights[" + std::to_string(m_lightID) + "].diffuse", m_color * m_diffuse);
        shader->setUniformVec3("pointLights[" + std::to_string(m_lightID) + "].specular", m_color * m_specular);
    }

}