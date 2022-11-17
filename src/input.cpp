#include "input.h"

void Input::pressKey(int key, float deltaTime)
{
    m_actions[m_keys[key]](deltaTime);
};


void Input::pressEventKey(int key, bool isPressed, float deltaTime)
{
    if (m_keys.find(key) != m_keys.end())
    {
        if (m_actionStatus[m_keys[key]] == RELEASED)
        {
            m_actionStatus[m_keys[key]] = isPressed ? PRESSED : RELEASED;
        }
        else if (m_actionStatus[m_keys[key]] == PRESSED)
        {
            if (!isPressed)
            {
                m_actionStatus[m_keys[key]] = DEPRESSED;
                m_actions[m_keys[key]](deltaTime);
            }
        }
        else if (m_actionStatus[m_keys[key]] == DEPRESSED)
        {
            m_actionStatus[m_keys[key]] = isPressed ? PRESSED : RELEASED;
        }
    }
}


void Input::updateCursor(float newX, float newY)
{
    if (!m_mouseCapture)
    {
        // LOG_DEBUG("Mouse not captured (mouse should be free from the window)");
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