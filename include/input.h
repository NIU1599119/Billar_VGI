#pragma once

#include <functional>
#include <map>
#include <vector>

#include "debug.h"

enum ACTIONS {
    MOVE_FORWARDS,
    MOVE_BACKWARDS,
    MOVE_LEFT,
    MOVE_RIGHT,
    EXIT
};


class Input {
public:


    ///////// KEYBOARD /////////

    void setKeyAction(ACTIONS action, int key, bool isPolling = true)
    {
        m_keys[key] = action;
        if (isPolling)
        {
            m_pollingKeys.push_back(key);
        }
    }

    int getKeyAction(int key) { return m_keys[key]; };

    void removeKeyAction(int key)
    {
        auto iter = m_pollingKeys.begin();
        for (iter; iter != m_pollingKeys.end(); iter++)
        {
            if (*iter == key)
            {
                break;
            }
        }
        m_pollingKeys.erase(iter);
        m_keys.erase(key);
    }

    void setActionFunction(ACTIONS action, std::function<void(float)> func) { m_actions[action] = func; };

    void activate(ACTIONS action, float deltaTime) { m_actions[action](deltaTime); };

    void removeActionFunction(ACTIONS action) { m_actions.erase(action); };

    std::vector<int> getPollingKeys() { return m_pollingKeys; };

    void pressKey(int key, float deltaTime) { m_actions[m_keys[key]](deltaTime); };

    ////////// MOUSE ///////////

    void updateCursor(float newX, float newY);

    void setMouseCallback(std::function<void(float, float)> mouseCallback) { m_mouseCallback = mouseCallback; };

    void lockMouse() { m_mouseLock = true; };

    void unlockMouse() { m_mouseLock = false; m_mouseReset = true; };

private:
    ///////// KEYBOARD /////////
    std::map<int, ACTIONS> m_keys;
    // engine action to GLFW key

    std::map<ACTIONS, std::function<void(float)>> m_actions;
    // engine action to function

    // std::vector<ACTIONS> m_pollingKeys; // use this to iterate keys in polling function
    // std::vector<ACTIONS> m_callbackKeys; // use this to iterate keys in callback function?

    std::vector<int> m_pollingKeys;

    ////////// MOUSE ///////////
    float m_sensitivity = 0.1f;

    float m_mouseX;
    float m_mouseY;

    bool m_mouseReset = true;
    bool m_mouseLock = false;
    bool m_mouseOffset = true; // pass offset(true) or position(false)

    std::function<void(float, float)> m_mouseCallback;

};






