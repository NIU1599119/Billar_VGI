#include "window.h"
#include "debug.h"
#include "gl_utils.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WindowData* data = (WindowData *)glfwGetWindowUserPointer(window);
    data->width = width;
    data->height = height;
    
    GL(glViewport(0, 0, width, height));

    LOG_DEBUG("Resized window to %dx%d", width, height);
}

Window::Window(unsigned int width, unsigned int height, std::string title, bool vSync)
{
    m_data = WindowData{
        title,
        width,
        height,
        vSync,
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

    GLFWwindow* window = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), NULL, NULL);

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
