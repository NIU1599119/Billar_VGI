#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "input.h"

struct WindowData
{
    std::string title;
    unsigned int width;
    unsigned int height;
    bool vSync;
    bool fullScreen;

    Input input;
    // input is passed to glfw as it only has window user pointer
};


class Window { 
public:
    Window(unsigned int width=800, unsigned int height=600, std::string title="Default", bool vSync=true, bool fullScreen=false);
    ~Window();
    bool initWindow();

    void update();

    GLFWwindow* getGLFWwindow() { return m_window; };

    bool shouldClose() { return glfwWindowShouldClose(m_window); };

    void close() { glfwSetWindowShouldClose(m_window, true); }

    void setVSync(bool enable);
    void setFullscreen(bool enable);

    Input* getInput() { return &m_data.input; }
    void processInput(float deltaTime);
    void resizeWindow(int width, int height);
    float getWidth() { return m_data.width; }
    float getHeight() { return m_data.height; }
private:
    GLFWwindow* m_window;
    GLFWmonitor* m_monitor;
    WindowData m_data;
};