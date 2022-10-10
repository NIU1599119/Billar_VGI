#include <glad/glad.h>
#include <string>
#include "gl_utils.h"

class ShaderProgram {
public:
    ShaderProgram() {};
    ShaderProgram(std::string vertexPath, std::string fragmentPath) : m_vertexPath(vertexPath), m_fragmentPath(fragmentPath) {};

    bool compileShaders();
    void activate() { GL(glUseProgram(m_shaderProgram)); };

    unsigned int getProgram() { return m_shaderProgram; };

private:
    unsigned int m_shaderProgram;

    std::string m_vertexPath;
    std::string m_fragmentPath;

    bool checkProgramLinking(unsigned int programID);
    bool checkShaderCompilation(unsigned int shaderID, unsigned int type);
    
};

std::string readShaderFile(std::string dir);

