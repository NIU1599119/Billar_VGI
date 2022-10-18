#include "input.h"


void Input::updateCursor(float newX, float newY)
{
    if (m_mouseLock)
    {
        // LOG_DEBUG("Mouse Locked (mouse should be free from the window)");
        return;
    }

    if (m_mouseReset)
    {
        m_mouseX = newX;
        m_mouseY = newY;
        m_mouseReset = false;
        // LOG_DEBUG("Mouse position reset");
    }

    if (!m_mouseOffset)
    {
        m_mouseX = newX;
        m_mouseY = newY;
        m_mouseCallback(m_mouseX, m_mouseY);
        return;
    }

    float xoffset = newX - m_mouseX;
    float yoffset = m_mouseY - newY; // reversed since y-coordinates range from bottom to top
    m_mouseX = newX;
    m_mouseY = newY;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    m_mouseCallback(xoffset, yoffset);
}