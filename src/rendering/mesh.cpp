#include "rendering/flexibleMesh.h"
#include "rendering/mesh.h"
#include "debug.h"
#include "gl_utils.h"
#include <cstdlib>

void Mesh::setVertexData(float* vertexData, unsigned int nVertex)
{
    m_vertexData = vertexData;
    m_nVertex = nVertex;
}

void Mesh::setVertexAtr(GLsizei stride, std::vector<AttributeData> attributeData)
{
    m_stride = stride;
    m_attributeData = attributeData;
}

void Mesh::setVertex(float* vertexData, unsigned int nVertex, GLsizei stride, std::vector<AttributeData> attributeData)
{
    m_vertexData = vertexData;
    m_nVertex = nVertex;
    m_stride = stride;
    m_attributeData = attributeData;
}


void Mesh::create()
{
    ASSERT(m_VAO == 0);
    ASSERT(m_VBO == 0);
    ASSERT(m_attributeData.size() > 0);

    // generate VAO (Vertex Array Object)
    unsigned int VAO;
    GL(glGenVertexArrays(1, &VAO));
    // Bind the VAO
    GL(glBindVertexArray(VAO));

    // generate VBO (Vertex Buffer Objects)
    unsigned int VBO;
    GL(glGenBuffers(1, &VBO));
    // copy vertices to buffer
    GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, m_nVertex*m_stride, m_vertexData, GL_STATIC_DRAW));


    // GLint prevPos = 0;
    for (int i = 0; i < m_attributeData.size(); i++)
    {
        void* offset = m_attributeData[i].offset;
        GLint size = m_attributeData[i].size;
        GL(glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, m_stride, offset));
        GL(glEnableVertexAttribArray(i));
    }
    
    m_VAO = VAO;
    m_VBO = VBO;

}

void Mesh::draw()
{
    GL(glBindVertexArray(m_VAO));                   // activate the VAO

    redraw();
}

void Mesh::redraw()
{
    GL(glDrawArrays(GL_TRIANGLES, 0, m_nVertex));   // assume that the VAO has been activated previously and draw all triangles
}

namespace Rendering {
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        m_vertices = vertices;
        m_indices = indices;
        m_textures = textures;

        setupMesh();
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
        GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));

        // vertex normals
        GL(glEnableVertexAttribArray(1));
        GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal)));

        // vertex texture coords
        GL(glEnableVertexAttribArray(2));
        GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, TexCoords)));

        // bind an invalid VAO
        GL(glBindVertexArray(0));
    }

    void Mesh::draw(Shader *shader)
    {
        unsigned int diffuseNr = 0;
        unsigned int specularNr = 0;
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
            shader->setUniformInt(("material." + name + number).c_str(), i);
            GL(glBindTexture(GL_TEXTURE_2D, m_textures[i].id));
        }
        GL(glActiveTexture(GL_TEXTURE0));

        // draw mesh
        GL(glBindVertexArray(m_VAO));
        GL(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0));
        GL(glBindVertexArray(0));
    }
}