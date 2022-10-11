
#include "shaderProgram.h"
#include "gl_utils.h"

#include <fstream>
#include <iostream>


std::string readShaderFile(std::string dir) {
    std::string fileData;
    std::ifstream file;

    file.open(dir);

    std::string line;
    while(!file.eof())
    {
        std::getline(file, line);
        fileData.append(line + "\n");
    }

    // std::cout << fileData;

    return fileData;
}


bool ShaderProgram::compileShaders() {
    
    std::string vertShaderStr = readShaderFile(m_vertexPath);
    std::string fragShaderStr = readShaderFile(m_fragmentPath);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    // vertex shader
    unsigned int vertexShader;
    GL(vertexShader = glCreateShader(GL_VERTEX_SHADER));
    GL(glShaderSource(vertexShader, 1, &vertShaderSrc, NULL));
    GL(glCompileShader(vertexShader));
    // check compiling
    bool success = checkShaderCompilation(vertexShader, GL_VERTEX_SHADER);
    if (!success) return false;    

    // fragment shader
    unsigned int fragmentShader;
    GL(fragmentShader = glCreateShader(GL_FRAGMENT_SHADER));
    GL(glShaderSource(fragmentShader, 1, &fragShaderSrc, NULL));
    GL(glCompileShader(fragmentShader));
    // check compiling
    success = checkShaderCompilation(vertexShader, GL_FRAGMENT_SHADER);
    if (!success) return false;

    // compile shaders
    unsigned int shaderProgram;
    GL(shaderProgram = glCreateProgram());

    GL(glAttachShader(shaderProgram, vertexShader));
    GL(glAttachShader(shaderProgram, fragmentShader));
    GL(glLinkProgram(shaderProgram));
    // check linking
    success = checkProgramLinking(shaderProgram);
    if (!success) return false;
    
    GL(glUseProgram(shaderProgram));
    GL(glDeleteShader(vertexShader));
    GL(glDeleteShader(fragmentShader));

    m_shaderProgram = shaderProgram;

    // std::cout << "ShaderProgram:" << m_shaderProgram << std::endl;

    return true;
}


bool ShaderProgram::checkShaderCompilation(unsigned int shaderID, unsigned int type)
{
    int  success;
    char infoLog[512];
    GL(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success));
    if (!success)
    {
        GL(glGetShaderInfoLog(shaderID, 512, NULL, infoLog));
        std::string typeStr;
        switch (type)
        {
        case GL_VERTEX_SHADER:
            typeStr = "VERTEX";
            break;
        case GL_FRAGMENT_SHADER:
            typeStr = "FRAGMENT";
            break;
        case GL_GEOMETRY_SHADER:
            typeStr = "GEOMETRY";
            break;
        default:
            typeStr = "UNKNOWN";
            break;
        }
        std::cout << "ERROR::SHADER::" << typeStr << "::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }
    return true;
}

bool ShaderProgram::checkProgramLinking(unsigned int programID)
{
    int success;
    char infoLog[512];
    GL(glGetProgramiv(programID, GL_LINK_STATUS, &success));
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    return true;
}