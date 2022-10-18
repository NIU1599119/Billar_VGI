#pragma once
#include <vector>

#include <glad/glad.h>

struct AttributeData {
    AttributeData();
    AttributeData(GLint atrSize, void* atrOffset) : size(atrSize), offset(atrOffset) {};

    GLint size;
    void* offset;
};


class Mesh {
/*
    This class is temporal, it may be changed to enable model imports
*/
public:
    Mesh() {};
    ~Mesh() {};

    void setVertexData(float* vertexData, unsigned int size);
    void setVertexAtr(GLsizei stride, std::vector<AttributeData> attributeData);

    void setVertex(float* vertexData, unsigned int size, GLsizei stride, std::vector<AttributeData> attributeData);

    void create();

    void draw();

    void redraw();


private:
    // vertex information
    float* m_vertexData = nullptr;      // vertex data array
    unsigned int m_nVertex;             // number of vertices
    GLsizei m_stride;                   // size of each vertex (x*sizeof(float)=stride)
    std::vector<AttributeData> m_attributeData;
    // saves the attribute data for position, color, texture, etc.
    //      layout location of attribute is defined by index in vector
    //      size is number of float values defined for the attribute
    //      offset is the index of the attribute in each stride


// opengl stuff
    // vertex info ids
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
};

