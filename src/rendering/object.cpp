#include "rendering/object.h"


namespace Rendering {
    void Object::draw(Shader* shader, glm::mat4& view, glm::mat4& projection, glm::vec3& cameraPosition)
    {
        glm::mat4 translate = glm::mat4(1.0f);
        translate = glm::translate(translate, m_position);
        glm::mat4 orientation = glm::mat4_cast(m_orientation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scaling*m_modelScaling /* component wise multiplication : (x1*x2, y1*y2, z1*z2) */);
        glm::mat4 model = translate * orientation * scale;

        shader->activate();
        shader->setUniformMat4("model", model);
        shader->setUniformMat4("normalRotation", orientation);
        shader->setUniformMat4("view", view);
        shader->setUniformVec3("viewPos", cameraPosition);
        shader->setUniformMat4("projection", projection);

        shader->setUniformFloat("material.shininess", 32.0f);

        m_model->draw(shader);
    }
    void Object::draw(Shader* shader)
    {
        glm::mat4 translate = glm::mat4(1.0f);
        translate = glm::translate(translate, m_position);
        glm::mat4 orientation = glm::mat4_cast(m_orientation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scaling * m_modelScaling /* component wise multiplication : (x1*x2, y1*y2, z1*z2) */);
        glm::mat4 model = translate * orientation * scale;

        shader->activate();
        shader->setUniformMat4("model", model);
        shader->setUniformMat4("normalRotation", orientation);

        shader->setUniformFloat("material.shininess", 32.0f);

        m_model->draw(shader);
    }
}