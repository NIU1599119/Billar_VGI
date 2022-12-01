#pragma once

// basic includes
#include <string>
#include <vector>
#include "debug.h"

// Inlclude models
#include "rendering/model.h"
#include "rendering/object.h"
#include "camera/camera.h"

namespace Rendering {

    class ModelController {
    public:
        ModelController() {
            m_projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f); /// this should be updated when window changes size
            m_modelShader = Shader("shaders/model.vert", "shaders/model.frag");
        }
        void addObject(const std::string& path, glm::vec3& position, float scale, float angle, glm::vec3& direction);
        void drawObjects(Camera& camera);
    private:
        std::vector<Rendering::Object> m_objectArray;
        glm::mat4 m_projection;
        Shader m_modelShader;
    };
};