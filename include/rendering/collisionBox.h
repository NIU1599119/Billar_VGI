#pragma once

// general
#include <glm/glm.hpp>

// renderizado
#include "rendering/shader.h"
#include "rendering/mesh.h"

// fisicas
#include "btBulletDynamicsCommon.h"


namespace Rendering {
    class CollisionBox
    {
    public:
        CollisionBox(Mesh* mesh = nullptr) : m_mesh(mesh) { if(mesh == nullptr) generateCubeMesh();};

        //////// RENDERING ////////

        void draw(Shader* collisionRenderingShader, glm::mat4& view, glm::mat4& projection);

        void setPosition(glm::vec3 position) { m_position = position; };
        void setScale(glm::vec3 scale) { m_scale = scale; };

        glm::vec3 getPosition() { return m_position; };
        glm::vec3 getScale() { return m_scale; };

        void setColor(glm::vec3 color) { m_color = color; };
        glm::vec3 getColor() { return m_color; };

        void setMesh(Mesh* mesh) { m_mesh = mesh; };
        Mesh* getMesh() { return m_mesh; };

        //////// PHISICS ////////

        void initializeBullet(btAlignedObjectArray<btCollisionShape*>& collisionShapes, btDiscreteDynamicsWorld* dynamicsWorld);

        void setFriction(double friction) { m_friction = friction; }
        void setRestitution(double restitution) { m_restitution = restitution; }
        // NOTE: cant update this at runtime

    private:
        int m_lightID;

        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::vec3 m_color = glm::vec3(1.0f);

        ///////////// RENDERING CUBE /////////////
        void generateCubeMesh();

        Mesh* m_mesh;

        //////// PHISICS ////////
        double m_friction = .8165;      // defaults for table
        double m_restitution = .513;
    };
};