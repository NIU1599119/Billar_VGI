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
    SWITCH_MOUSE,
    LEFT_CLICK,
    PUSH_BALL,
    ACCELERATE_BALL,
    EXIT
};

enum STATUS {
    PRESSED,
    DEPRESSED,  // usually executes an event
    RELEASED
};

class Input {
public:

    // struct eventKey {
    //     int key;
    //     bool isPressed;
    // };

    ///////// KEYBOARD /////////

    void setKeyAction(ACTIONS action, int key, bool isPolling = true)
    {
        m_keys[key] = action;
        m_actionStatus[action] = RELEASED;
        if (isPolling)
        {
            auto iter = m_pollingKeys.begin();
            for (iter; iter != m_pollingKeys.end(); iter++)
            {
                if (*iter == key)
                {
                    break;
                }
            }
            if (iter != m_pollingKeys.end())
            {
                m_pollingKeys.erase(iter);
            }
            m_pollingKeys.push_back(key);
        }
        else// isEvent
        {
            // eventKey ek;
            // ek.key = key;
            // ek.isPressed = false;
            // m_eventKeys.push_back(ek);
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
        if (iter != m_pollingKeys.end())
        {
            m_pollingKeys.erase(iter);
        }
        m_keys.erase(key);
    }

    void setActionFunction(ACTIONS action, std::function<void(float)> func) { m_actions[action] = func; };

    void activate(ACTIONS action, float deltaTime) { m_actions[action](deltaTime); };

    void removeActionFunction(ACTIONS action) { m_actions.erase(action); };

    std::vector<int>* getPollingKeys() { return &m_pollingKeys; };
    // std::vector<eventKey>* getEventKeys() { return &m_eventKeys; };

    void pressKey(int key, float deltaTime);

    void pressEventKey(int key, bool isPressed, float deltaTime);

    void setActionStatus(ACTIONS action, STATUS status) { m_actionStatus[action] = status; };
    STATUS getActionStatus(ACTIONS action) { return m_actionStatus[action]; };

    ////////// MOUSE ///////////

    void updateCursor(float newX, float newY);

    void setMouseCallback(std::function<void(float, float)> mouseCallback, bool relative=true) { m_mouseCallback = mouseCallback; m_mouseOffset = relative; };
    void removeMouseCallback() { m_mouseCallback = [](float x, float y){}; };

    void captureMouse() { m_mouseCapture = true; };

    void uncaptureMouse() { m_mouseCapture = false; m_mouseReset = true; };

    bool mouseIsCaptured() { return m_mouseCapture; };

private:
    ///////// KEYBOARD /////////
    std::map<int, ACTIONS> m_keys;
    // engine action to GLFW key

    std::map<ACTIONS, std::function<void(float)>> m_actions;
    // engine action to function

    // std::vector<ACTIONS> m_pollingKeys; // use this to iterate keys in polling function
    // std::vector<ACTIONS> m_callbackKeys; // use this to iterate keys in callback function?

    std::vector<int> m_pollingKeys;

    std::map<ACTIONS, STATUS> m_actionStatus;

    // std::vector<eventKey> m_eventKeys;

    ////////// MOUSE ///////////
    float m_sensitivity = 0.1f;

    float m_mouseX;
    float m_mouseY;

    bool m_mouseReset = true;
    bool m_mouseCapture = false;
    bool m_mouseOffset = true; // pass offset(true) or position(false)

    std::function<void(float, float)> m_mouseCallback;

};


