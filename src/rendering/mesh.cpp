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

