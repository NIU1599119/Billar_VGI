#include "window.h"
#include "debug.h"
#include "gl_utils.h"


// callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WindowData* data = (WindowData *)glfwGetWindowUserPointer(window);
    data->width = width;
    data->height = height;
    
    GL(glViewport(0, 0, width, height));

    LOG_DEBUG("Resized window to %dx%d", width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    WindowData* data = (WindowData *)glfwGetWindowUserPointer(window);
    Input* input = &data->input;

    if (action == GLFW_PRESS)
    {
        input->pressEventKey(key, true, 0.0f);
    }
    else if (action == GLFW_RELEASE)
    {
        input->pressEventKey(key, false, 0.0f);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ((WindowData *)glfwGetWindowUserPointer(window))->input.updateCursor(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    WindowData* data = (WindowData *)glfwGetWindowUserPointer(window);
    Input* input = &data->input;

    if (action == GLFW_PRESS)
    {
        input->pressEventKey(button, true, 0.0f);
    }
    else if (action == GLFW_RELEASE)
    {
        input->pressEventKey(button, false, 0.0f);
    }
}

// Window Class

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
    m_monitor = glfwGetPrimaryMonitor();

    if(!m_data.fullScreen)
        window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);
    else
    {
        const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);
        window = glfwCreateWindow(mode->width, mode->height, m_data.title.c_str(), m_monitor, NULL);
    }

    if (window == NULL)
    {
        LOG_ERROR("Failed to create GLFW window");
        return false;
    }

    glfwSetWindowUserPointer(window, &m_data);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // input
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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

void Window::processInput(float deltaTime)
{
    Input* input = getInput();

    std::vector<int>* keys = input->getPollingKeys();
    for (int i = 0; i < keys->size(); i++)
    {
        if (glfwGetKey(m_window, (*keys)[i]) == GLFW_PRESS)
            input->pressKey((*keys)[i], deltaTime);
    }
}

void Window::resizeWindow(int width, int height)
{
    glfwSetWindowSize(m_window, width, height);
}

void Window::update()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}


void Window::setVSync(bool enable)
{
    if (enable)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
    
    m_data.vSync = enable;
}

void Window::setFullscreen(bool enable)
{
    if (enable)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);
        glfwSetWindowMonitor(m_window, m_monitor, 0, 0, mode->width, mode->height, 0);
    }
    else
    {
        glfwSetWindowMonitor(m_window, nullptr, 0, 0, m_data.width, m_data.height, 0);
    }
    m_data.fullScreen = enable;
}