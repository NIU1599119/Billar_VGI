// https://stackoverflow.com/questions/14486291/how-to-draw-line-in-opengl
#pragma once
#include <glad/glad.h>
#include "gl_utils.h"
#include "debug.h"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rendering/shader.h"

namespace Rendering
{
    class Line {
        int shaderProgram;
        unsigned int VBO, VAO, EBO;
        std::vector<float> vertices;
        glm::vec3 startPoint;
        glm::vec3 endPoint;
        // glm::mat4 MVP;
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 lineColor;
    public:
        Line(glm::vec3 start, glm::vec3 end);

        int setViewProjection(glm::mat4 v, glm::mat4 p);

        int setColor(glm::vec3 color);

        void setPos(glm::vec3 start, glm::vec3 end);

        int draw();

        ~Line();
    };

}