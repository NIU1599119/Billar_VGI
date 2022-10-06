#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdio.h>
#include <iostream>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

/////////// librerias originales ///////////
// #include "engine.h"
// #include "gl_utils.h"
// #include "window.h"
// #include "shader.h"
// #include "renderer2d.h"



// funciones
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// shaders ejemplo
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform vec2 dPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x + dPos.x, aPos.y + dPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";




int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Test", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    glViewport(0,0,800,600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // triangle vertices
    // float vertices[] = {
    //     -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.0f,  0.5f, 0.0f
    // };

    // sqare vertices without EBO
    // float vertices[] = {
    //     // first triangle
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f,  0.5f, 0.0f,  // top left 
    //     // second triangle
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f,  // bottom left
    //     -0.5f,  0.5f, 0.0f   // top left
    // }; 

    // square vertices with (EBO)
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
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



    // set vertices attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
        // sould check compiling here
    
    // fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
        // sould check compiling here

    // compile shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
        // should check linking
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    bool drawTriangles = true;
    float xPos = 0;
    float yPos = 0;

    unsigned int nFrame = 0;
    while(!glfwWindowShouldClose(window))
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
            processInput(window);

        }


        // triangle code here:
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);                         // Binding VAO automatically binds EBO
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  // EBO is bound to VAO, so this is not needed
        if (drawTriangles)
        {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                                //       ^ indices size
        }
        glBindVertexArray(0);


        ImGui::Begin("My name is window, ImGUI window");
        ImGui::Text("Hello there!");
        ImGui::Checkbox("Draw Triangle", &drawTriangles);
        ImGui::SliderFloat("xPos", &xPos, -1.0f, 1.0f);
        ImGui::SliderFloat("yPos", &yPos, -1.0f, 1.0f);
        ImGui::End();

        glUseProgram(shaderProgram);
        glUniform2f(glGetUniformLocation(shaderProgram, "dPos"), xPos, yPos);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        nFrame++;
    }



    glfwTerminate();
    return 0;


    //////////////////// CODIGO ORIGINAL : NO USAR ////////////////////

    // glm::vec3 vector(1, 0, 1);
    // printf("%f %f %f\n", vector.x, vector.y, vector.z);

    // Window window("OpenGL Boilerplate!!!", 960, 540, true);
    // window.Create();

    // Shader shader("Basic", "res/basic.vert", "res/basic.frag");
    // if (!shader.Create())
    //     return 1;

    // Renderer2D renderer(shader, window.GetDataPointer(), 100);
    // renderer.Create();

    // // Setup ImGui

    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();

    // ImGui::StyleColorsDark();

    // ImGui_ImplGlfw_InitForOpenGL(window.GetGlfwWindow(), true);
    // ImGui_ImplOpenGL3_Init("#version 130");

    // bool show_demo_window = true;

    // while (!window.ShouldClose())
    // {
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     renderer.Start();
        
    //     for (int i = 0; i < 1000; i++) {
    //         float x = (float)rand()/(float)(RAND_MAX);
    //         float y = (float)rand()/(float)(RAND_MAX);
    //         renderer.DrawQuad(glm::vec2(x * window.GetSize().x, y * window.GetSize().y), glm::vec2(10.0f, 10.0f), glm::vec4(x, y * 0.5f, 0.2f, 1.0f));
    //     }

    //     renderer.End();

    //     ImGui_ImplOpenGL3_NewFrame();
    //     ImGui_ImplGlfw_NewFrame();
    //     ImGui::NewFrame();

    //     renderer.DrawImGui();

    //     if (show_demo_window)
    //         ImGui::ShowDemoWindow(&show_demo_window);

    //     ImGui::Render();
    //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //     window.Update();
    // }

    // renderer.Destroy();

    // shader.Destroy();

    // window.Destroy();
}