#include "window.h"
#include "debug.h"
#include "gl_utils.h"

void processInput(GLFWwindow* window, Input* input, float deltaTime)
{
    std::vector<Input::eventKey>* ekeys = input->getEventKeys();
    for (int i = 0; i < ekeys->size(); i++)
    {
        if (glfwGetKey(window, (*ekeys)[i].key) == GLFW_RELEASE && (*ekeys)[i].isPressed)
        {
            input->pressKey((*ekeys)[i].key, deltaTime);
            (*ekeys)[i].isPressed = false;
        }
        else if (glfwGetKey(window, (*ekeys)[i].key) == GLFW_PRESS)
            (*ekeys)[i].isPressed = true;
    }

    std::vector<int>* keys = input->getPollingKeys();
    for (int i = 0; i < keys->size(); i++)
    {
        if (glfwGetKey(window, (*keys)[i]) == GLFW_PRESS)
            input->pressKey((*keys)[i], deltaTime);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WindowData* data = (WindowData *)glfwGetWindowUserPointer(window);
    data->width = width;
    data->height = height;
    
    GL(glViewport(0, 0, width, height));

    LOG_DEBUG("Resized window to %dx%d", width, height);
}

Window::Window(unsigned int width, unsigned int height, std::string title, bool vSync, bool fullScreen)
{
    m_data = WindowData{
        title,
        width,
        height,
        vSync,
        fullScreen
    };
}

Window::~Window()
{
    if (m_window != nullptr)
        glfwDestroyWindow(m_window);
}


bool Window::initWindow()
{
    if (!glfwInit())
    {
        LOG_ERROR("Failed to initialize GLFW");
        return false;
    }
        

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;

    if(!m_data.fullScreen)
        window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);
    else
    {
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), primary, NULL);
    }

    if (window == NULL)
    {
        LOG_ERROR("Failed to create GLFW window");
        return false;
    }

    glfwSetWindowUserPointer(window, &m_data);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to initialize GLAD");
        return false;
    }

    m_window = window;

    setVSync(m_data.vSync);

    return true;
}



void Window::update()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}


void Window::setVSync(bool enabled)
{
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
    
    m_data.vSync = enabled;
}
