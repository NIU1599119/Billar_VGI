#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <iostream>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "gl_utils.h"
#include "shaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


// funciones
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// // shaders ejemplo
// const char *vertexShaderSource = "#version 330 core\n"
//     "layout (location = 0) in vec3 aPos;\n"
//     "uniform vec2 dPos;\n"
//     "void main()\n"
//     "{\n"
//     "   gl_Position = vec4(aPos.x + dPos.x, aPos.y + dPos.y, aPos.z, 1.0);\n"
//     "}\0";

// const char *fragmentShaderSource = "#version 330 core\n"
//     "out vec4 FragColor;\n"
//     "void main()\n"
//     "{\n"
//     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//     "}\0";

// shader files
std::string vertexDir = "shaders/default.vert";
std::string fragmentDir = "shaders/default.frag";

std::string textureDir = "textures/container.jpg";


int main()
{

    Window window(1200, 900, "Billar");

    if (!window.initWindow()) {
        return 1;
    }

    // // square vertices with (EBO)
    // float vertices[] = {
    //     /////////////////////// square dots
    //      0.5f,  0.5f, 0.0f,  // top right
    //      0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //     -0.5f,  0.5f, 0.0f,  // top left
    //     /////////////////////// triangle dots
    //     -0.5f, -0.5f, 0.0f,  //
    //      0.5f, -0.5f, 0.0f,  //
    //      0.0f,  0.5f, 0.0f   //
    // };

    // float vertices2[] = {
    //     // positions         // colors
    //      0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    //     -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
    //      0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // top
    //     -1.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f    // top to the left
    // };

    float vertices[] = {
        /////////////////////// square dots
        // positions        // colors           // textures
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,     // top right
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,     // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,     // bottom left
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,     // top left
        /////////////////////// triangle dots
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,     // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,     // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f      // top
    };
    

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
        0, 2, 3,   // second triangle
    };
    unsigned int indices2[] = {
        4, 5, 6    // another triangle
    };


    // generate VAO (Vertex Array Object)
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Bind the VAO
    glBindVertexArray(VAO);


    // generate VBO (Vertex Buffer Objects)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // copy vertices to buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // generate EBO (Element Buffer Object)
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // bind the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set vertices attributes pointers for previously bound VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // generate VAO2 (Vertex Array Object)
    unsigned int VAO2;
    glGenVertexArrays(1, &VAO2);

    // Bind the VAO2
    glBindVertexArray(VAO2);

    // generate EBO2 (Element Buffer Object)
    unsigned int EBO2;
    glGenBuffers(1, &EBO2);

    // bind the EBO2
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

    // set vertices attributes pointers for previously bound VAO2
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);


    ShaderProgram shaderProgram(vertexDir, fragmentDir);

    if(!shaderProgram.compileShaders())
    {
        return 1;
    }


    // textures
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(textureDir.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        // failed
    }
    stbi_image_free(data);





    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");


    bool drawTriangles = true;
    float xPos = 0;
    float yPos = 0;

    unsigned int nFrame = 0;
    while(!window.shouldClose())
    {
        // rendering commands here
        glClearColor(0.3f, 0.3f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ImGui newframe
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!io.WantCaptureMouse)
        {
            // input handling here
            processInput(window.getGLFWwindow());

        }


        // activate shader
        // glUseProgram(shaderProgram);
        shaderProgram.activate();


        // add uniform (modify position)
        glUniform2f(glGetUniformLocation(shaderProgram.getProgram(), "dPos"), xPos, yPos);

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram.getProgram(), "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);


        if (!drawTriangles)
        {
            glBindTexture(GL_TEXTURE_2D, texture);
            GL(glBindVertexArray(VAO));                         // Binding VAO automatically binds EBO
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);  // EBO is bound to VAO, so this is not needed
            GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*) 0));
                                //       ^ indices size
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, texture);
            GL(glBindVertexArray(VAO2));
            GL(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*) 0));
        }


        ImGui::Begin("My name is window, ImGUI window");
        ImGui::Text("Hello there!");
        ImGui::Checkbox("Draw Triangle(T), Draw Square(F)", &drawTriangles);
        ImGui::SliderFloat("xPos", &xPos, -1.0f, 1.0f);
        ImGui::SliderFloat("yPos", &yPos, -1.0f, 1.0f);
        ImGui::Text("Frame number %u", nFrame);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // check and call events and swap the buffers

        window.update();

        nFrame++;
    }


    glfwTerminate();
    return 0;
}