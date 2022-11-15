#pragma once

#include "rendering/shader.h"
#include <glm/glm.hpp>

#include "rendering/mesh.h"

namespace Rendering {
    class LightPoint
    {
    public:
        LightPoint(int id, bool generateMesh = false) : m_lightID(id), m_mesh(nullptr) { if(generateMesh) generateCubeMesh(); };

        void draw(Shader* lightRenderingShader, glm::mat4& view, glm::mat4& projection);

        void updateShader(Shader* targetShader);

        void drawAndUpdate(Shader* lightRenderingShader, Shader* targetShader);

        void setPosition(glm::vec3 position) { m_position = position; };

        glm::vec3 getPosition() { return m_position; };
        glm::vec3* getColor() { return &m_color; };

        void setMesh(Mesh* mesh) { m_mesh = mesh; };
        Mesh* getMesh() { return m_mesh; };

    private:
        int m_lightID;

        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        float m_constant = 1.0f;
        float m_linear = 0.09f;
        float m_cuadratic = 0.0032f;

        glm::vec3 m_ambient = glm::vec3(0.1f);
        float m_diffuse = 0.7f;
        float m_specular = 1.0f;

        glm::vec3 m_color = glm::vec3(1.0f);

        ///////////// RENDERING CUBE /////////////
        void generateCubeMesh();

        Mesh* m_mesh;
    };
};