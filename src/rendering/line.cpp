#include "rendering/line.h"


namespace Rendering
{

    Line::Line(glm::vec3 start, glm::vec3 end) {

        startPoint = start;
        endPoint = end;
        lineColor = glm::vec3(1,1,1);
        // MVP = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);

        std::string vertexShaderSource = Rendering::readShaderFile("shaders/line.vert");
        std::string geometryShaderSource = Rendering::readShaderFile("shaders/line.geom");
        std::string fragmentShaderSource = Rendering::readShaderFile("shaders/line.frag");

        const char *vertShaderSrc = vertexShaderSource.c_str();
        const char *geomShaderSrc = geometryShaderSource.c_str();
        const char *fragShaderSrc = fragmentShaderSource.c_str();

        // vertex shader
        int vertexShader = GL(glCreateShader(GL_VERTEX_SHADER));
        GL(glShaderSource(vertexShader, 1, &vertShaderSrc, NULL));
        GL(glCompileShader(vertexShader));
        // check for shader compile errors

        // geometry shader
        int geometryShader = GL(glCreateShader(GL_GEOMETRY_SHADER));
        GL(glShaderSource(geometryShader, 1, &geomShaderSrc, NULL));
        GL(glCompileShader(geometryShader));
        // check for shader compile errors

        // fragment shader
        int fragmentShader = GL(glCreateShader(GL_FRAGMENT_SHADER));
        GL(glShaderSource(fragmentShader, 1, &fragShaderSrc, NULL));
        GL(glCompileShader(fragmentShader));
        // check for shader compile errors

        // link shaders
        shaderProgram = GL(glCreateProgram());
        GL(glAttachShader(shaderProgram, vertexShader));
        GL(glAttachShader(shaderProgram, geometryShader));
        GL(glAttachShader(shaderProgram, fragmentShader));
        GL(glLinkProgram(shaderProgram));
        // check for linking errors

        GL(glDeleteShader(vertexShader));
        GL(glDeleteShader(geometryShader));
        GL(glDeleteShader(fragmentShader));

        vertices = {
            start.x, start.y, start.z,
            end.x, end.y, end.z
        };
        
        GL(glGenVertexArrays(1, &VAO));
        GL(glGenBuffers(1, &VBO));
        GL(glGenBuffers(1, &EBO));
        GL(glBindVertexArray(VAO));

        GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW));

        std::vector<unsigned int> indices = {
            0, 1
        };
        GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
        GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));


        GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
        GL(glEnableVertexAttribArray(0));

        GL(glBindBuffer(GL_ARRAY_BUFFER, 0);) 
        GL(glBindVertexArray(0);) 

    }

    int Line::setViewProjection(glm::mat4 v, glm::mat4 p) {
        view = v;
        projection = p;
        return 1;
    }

    int Line::setColor(glm::vec3 color) {
        lineColor = color;
        return 1;
    }

    void Line::setPos(glm::vec3 start, glm::vec3 end) {
        startPoint = start;
        endPoint = end;

        vertices = {
            start.x, start.y, start.z,
            end.x, end.y, end.z
        };
        GL(glBindVertexArray(VAO));
        GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data()));
    }

    int Line::draw() {

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    // wireframe rendering

        GL(glUseProgram(shaderProgram));
        GL(glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]));
        GL(glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]));
        GL(glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]));
        GL(glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]));
        // GL(glUniform3fv(glGetUniformLocation(shaderProgram, "startPos"), 1, &startPoint[0]));
        // GL(glUniform3fv(glGetUniformLocation(shaderProgram, "endPos"), 1, &endPoint[0]));

        // GL(glBindVertexArray(VAO));
        // GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        // GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
        GL(glBindVertexArray(VAO));
        // GL(glDrawArrays(GL_LINES, 0, 2));
        GL(glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0));

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    // wireframe rendering
        return 1;
    }

    Line::~Line() {

        GL(glDeleteVertexArrays(1, &VAO));
        GL(glDeleteBuffers(1, &VBO));
        GL(glDeleteBuffers(1, &EBO));
        GL(glDeleteProgram(shaderProgram));
    }


}