#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window() {};
    Window(unsigned int width, unsigned int height) : m_width(width), m_height(height) {};
    Window(unsigned int width, unsigned int height, std::string title) : m_width(width), m_height(height), m_title(title) {};
    ~Window();
    bool initWindow();

    void update();


    GLFWwindow* getGLFWwindow() { return m_window; };

    bool shouldClose() { return glfwWindowShouldClose(m_window); };


private:
    GLFWwindow* m_window;
    unsigned int m_width = 800, m_height = 600;
    std::string m_title = "Default";
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
