
#include "rendering/simpleModel.h"

namespace Rendering {


    void SimpleModel::draw(ShaderProgram* shader, glm::mat4 view, glm::mat4 projection)
    {
        if (m_texture != nullptr)
            m_texture->activate();

        // model transformations
        glm::mat4 translate = glm::mat4(1.0f);
        if (m_position != nullptr)
            translate = glm::translate(translate, *m_position);
        glm::mat4 rotate = glm::mat4_cast(m_orientation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scaling);
        glm::mat4 model = translate * rotate * scale;

        // shader activation
        shader->activate();
        shader->setUniformMat4("normalRotation", rotate);
        shader->setUniformMat4("model", model);
        shader->setUniformMat4("view", view);
        shader->setUniformMat4("projection", projection);


        m_mesh->draw();
    }

    void SimpleModel::redraw(ShaderProgram* shader)
    {

        /*
        this should only be executed when the position changes (with setPosition) after the execution of draw
        when redrawing you can change some things before the execution:
            - change position
            - change rotation
            - change a shader uniform

        things that are already set:
            - view
            - projection
            - shader
            - texture
            - mesh

        things that need recalculating:
            - model
        */

        // model transformations
        glm::mat4 translate = glm::mat4(1.0f);
        if (m_position != nullptr)
            translate = glm::translate(translate, *m_position);
        glm::mat4 rotate = glm::mat4_cast(m_orientation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scaling);
        glm::mat4 model = translate * rotate * scale;

        shader->setUniformMat4("normalRotation", rotate);
        shader->setUniformMat4("model", model);

        m_mesh->redraw();
        
    }

}