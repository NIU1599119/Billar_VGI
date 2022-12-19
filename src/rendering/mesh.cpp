#include "rendering/mesh.h"
#include "debug.h"
#include "gl_utils.h"
#include <cstdlib>


namespace Rendering {
    Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
    {
        m_vertices = vertices;
        m_indices = indices;
        m_textures = textures;

        setupMesh();

        m_vertices.clear();             // vaciar vector
        m_vertices.shrink_to_fit();     // desalocatarlo
        m_indicesSize = m_indices.size();
        m_indices.clear();
        m_indices.shrink_to_fit();
    }

    void Mesh::setupMesh()
    {
        GL(glGenVertexArrays(1, &m_VAO));
        GL(glGenBuffers(1, &m_VBO));
        GL(glGenBuffers(1, &m_EBO));

        GL(glBindVertexArray(m_VAO));

        GL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW));

        GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
        GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW));

        // vertex positions
        GL(glEnableVertexAttribArray(0));
        GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Position)));

        // vertex normals
        GL(glEnableVertexAttribArray(1));
        GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal)));

        // vertex texture coords
        GL(glEnableVertexAttribArray(2));
        GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, TexCoords)));

        // vertex tangent coords
        GL(glEnableVertexAttribArray(3));
        GL(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Tangent)));

        // bind an invalid VAO
        GL(glBindVertexArray(0));
    }

    void Mesh::dealocate()
    {
        GL(glBindVertexArray(0));
        GL(glDeleteBuffers(1, &m_EBO));
        GL(glDeleteBuffers(1, &m_VBO));
        GL(glDeleteVertexArrays(1, &m_VAO));
    }

    void Mesh::draw(Shader *shader)
    {
        unsigned int diffuseNr = 0;
        unsigned int specularNr = 0;
        unsigned int normalNr = 0;
        unsigned int roughnessNr = 0;
        for (unsigned int i = 0; i < m_textures.size(); i++)
        {
            GL(glActiveTexture(GL_TEXTURE0 + i));   // activate texture
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = m_textures[i].type;
            if (name == "texture_diffuse")
            {
                number = std::to_string(diffuseNr);
                diffuseNr++;
            }
            else if (name == "texture_specular")
            {
                number = std::to_string(specularNr);
                specularNr++;
            }
            else if (name == "texture_normal")
            {
                number = std::to_string(normalNr);
                normalNr++;
            }
            else if (name == "texture_roughness")
            {
                number = std::to_string(roughnessNr);
                roughnessNr++;
            }
            shader->setUniformInt(("material." + name + number).c_str(), i);
            GL(glBindTexture(GL_TEXTURE_2D, m_textures[i].id));
        }
        GL(glActiveTexture(GL_TEXTURE0));

        shader->setUniformBool("material.hasDiffuse",   diffuseNr   > 0);
        shader->setUniformBool("material.hasSpecular",  specularNr  > 0);
        shader->setUniformBool("material.hasNormal",    normalNr    > 0);
        shader->setUniformBool("material.hasRoughness", roughnessNr > 0);

        // draw mesh
        GL(glBindVertexArray(m_VAO));
        GL(glDrawElements(GL_TRIANGLES, m_indicesSize, GL_UNSIGNED_INT, 0));
        GL(glBindVertexArray(0));
    }
}