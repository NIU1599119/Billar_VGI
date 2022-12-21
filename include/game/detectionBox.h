#pragma once

#include "glm/glm.hpp"
#include "rendering/engine.h"
#include "rendering/primitives.h"
#include "EntityControllerSystem.h"

class DetectionBox
{
public:
    DetectionBox(Rendering::RenderEngine3D* renderEngine, EntityControllerSystem* ecs, std::function<void(int)> detectionCallback, glm::vec3 position = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f))
    : m_renderEngine(renderEngine),
      m_ecs(ecs),
      m_detectionCallback(detectionCallback),
      m_position(position),
      m_size(size.x/2, size.y/2, size.z/2)
    {
        m_renderID = renderEngine->createObject(Primitives::getCUBE(), glm::vec3(1.0f), false, renderEngine->getDebuggingShader());
        renderEngine->updateObjectPosition(m_renderID, position);
        renderEngine->updateObjectScaling(m_renderID, size);
    };

    void setPosition(glm::vec3 position) { m_position = position; };
    void setSize(glm::vec3 size) { m_size.x = size.x/2; m_size.y = size.y/2; m_size.z = size.z/2; };

    glm::vec3 getPosition() { return m_position; };
    glm::vec3 getSize() { return glm::vec3(m_size.x * 2, m_size.y * 2, m_size.z * 2); };

    // checks if there is a ball with a position in here
    bool checkBallInside(GAMEMODE gamemode)
    {
        int nBalls = 0;
        switch (gamemode)
        {
        case CLASSIC:
            nBalls = 16;
            break;
        case CARAMBOLA:
            nBalls = 3;
            break;
        case FREE_SHOTS:
            nBalls = 1;
            break;
        }

        m_detectedBallIDs.clear();

        for(int i = 0; i < nBalls; i++)
        {
            int ballID = m_ecs->getBallIdx(i);
            btDiscreteDynamicsWorldMt** dynamicsWorld = m_ecs->getDynamicsWorld();

            btCollisionObject* obj = (*dynamicsWorld)->getCollisionObjectArray()[ballID];
            btRigidBody* body = btRigidBody::upcast(obj);
            btTransform trans;
            if (body && body->getMotionState())
            {
                body->getMotionState()->getWorldTransform(trans);
            }
            else
            {
                trans = obj->getWorldTransform();
            }

            glm::vec3 pos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

            glm::vec3 minPos = m_position-m_size;
            glm::vec3 maxPos = m_position+m_size;
            if (pos.x > minPos.x && pos.y > minPos.y && pos.z > minPos.z && pos.x < maxPos.x && pos.y < maxPos.y && pos.z < maxPos.z)
            {
                // ball is inside box
                m_detectedBallIDs.push_back(i);
            }
        }

        m_isDetected = !m_detectedBallIDs.empty();  // si el vector no esta vacio significa que hay bolas dentro

        return m_isDetected;
    };

    void update()
    {
        #ifdef DEBUG
        m_renderEngine->updateObjectPosition(m_renderID, m_position);
        glm::vec3 actualSize = glm::vec3(m_size.x*2, m_size.y*2, m_size.z*2);
        m_renderEngine->updateObjectScaling(m_renderID, actualSize);
        Rendering::Shader* debugShader = m_renderEngine->getDebuggingShader();
        //debugShader->setUniformVec3("color", (m_isDetected) ? DETECTED : OUTSIDE);
        #endif
        for (int i = 0; i < m_detectedBallIDs.size(); i++)
        {
            m_detectionCallback(m_detectedBallIDs[i]);
        }
    }

    std::vector<int> getDetectedBallIDs() { return m_detectedBallIDs; };


private:
    int m_renderID;

    EntityControllerSystem* m_ecs;
    Rendering::RenderEngine3D* m_renderEngine;

    glm::vec3 m_position;
    glm::vec3 m_size;
    const glm::vec3 OUTSIDE = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 DETECTED = glm::vec3(0.0f, 1.0f, 0.0f);


    bool m_isDetected = false;
    std::vector<int> m_detectedBallIDs;

    std::function<void(int)> m_detectionCallback;
};