#pragma once

#include <glad/glad.h>
#include <string>
#include "gl_utils.h"
#include <glm/glm.hpp>
namespace Rendering
{
    class Shader {
    public:
        Shader() {};
        Shader(std::string vertexPath, std::string fragmentPath) : m_vertexPath(vertexPath), m_fragmentPath(fragmentPath) {};

        bool compileShaders();
        void activate() { GL(glUseProgram(m_shaderProgram)); };

        unsigned int getProgram() { return m_shaderProgram; };

        // uniforms
        // add more when needed

        void setUniformMat4(std::string name, glm::mat4 uniformMat4); // this is for the model view projection matrices mainly
        void setUniformVec3(std::string name, glm::vec3 uniformVec3);
        void setUniformFloat(std::string name, float uniformFloat);
        void setUniformInt(std::string name, int uniformInt);
        void setUniformBool(std::string name, bool uniformBool);


    private:
        unsigned int m_shaderProgram;

        std::string m_vertexPath;
        std::string m_fragmentPath;

        bool checkProgramLinking(unsigned int programID);
        bool checkShaderCompilation(unsigned int shaderID, unsigned int type);

    };

    std::string readShaderFile(std::string dir);
}
