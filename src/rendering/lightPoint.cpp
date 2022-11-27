#include "rendering/lightPoint.h"
#include "debug.h"

#include <glm/gtc/matrix_transform.hpp>


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

    void LightPoint::draw(Shader* shader, glm::mat4& view, glm::mat4& projection)
    {
        if (m_mesh == nullptr)
            return;
        glm::mat4 translate = glm::mat4(1.0f);
        translate = glm::translate(translate, m_position);
        glm::mat4 orientation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        glm::mat4 model = translate * orientation * scale;

        shader->activate();
        shader->setUniformVec3("lightColor", m_color);
        shader->setUniformMat4("model", model);
        shader->setUniformMat4("normalRotation", orientation);
        shader->setUniformMat4("view", view);
        shader->setUniformMat4("projection", projection);

        m_mesh->draw(shader);
    }

    void LightPoint::generateCubeMesh()
    {
        float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
        };
        std::vector<unsigned int> indices = {
            0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35
        };
        std::vector<Texture> textures;

        std::vector<Vertex> meshVertices;
        for (int i=0; i < 36; i++)
        {
            // i*3 + (0, 1, 2) for (x, y, z)
            int index = i*8;
            Vertex v;
            v.Position = glm::vec3(vertices[index], vertices[index+1], vertices[index+2]);
            v.Normal = glm::vec3(vertices[index+3], vertices[index+4], vertices[index+5]);
            v.TexCoords = glm::vec2(vertices[index+6], vertices[index+7]);

            meshVertices.push_back(v);
        }

        if (m_mesh != nullptr)
            delete m_mesh;
        m_mesh = new Mesh(meshVertices, indices, textures);
    }
}