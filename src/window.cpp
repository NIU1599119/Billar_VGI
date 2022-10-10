#include "window.h"
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


Window::~Window()
{

    glfwTerminate();

    if (m_window != nullptr)
        glfwDestroyWindow(m_window);
}


bool Window::initWindow()
{
    if (!glfwInit())
        throw std::runtime_error("GLFW failed to initialize.");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        return false;
    }

    glfwMakeContextCurrent(window);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return false;
    }

    m_window = window;


    return true;
}



void Window::update()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}