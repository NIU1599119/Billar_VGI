#include "rendering/collisionBox.h"

#include "debug.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Rendering {

    void CollisionBox::draw(Shader* shader, glm::mat4& view, glm::mat4& projection)
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );    // wireframe rendering

        if (m_mesh == nullptr)
            return;
        glm::mat4 translate = glm::mat4(1.0f);
        translate = glm::translate(translate, m_position);
        glm::mat4 orientation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);
        glm::mat4 model = translate * orientation * scale;

        shader->activate();
        shader->setUniformVec3("color", m_color);
        shader->setUniformMat4("model", model);
        shader->setUniformMat4("normalRotation", orientation);
        shader->setUniformMat4("view", view);
        shader->setUniformMat4("projection", projection);

        m_mesh->draw(shader);

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );    // disable wireframe rendering
    }

    void CollisionBox::initializeBullet(btAlignedObjectArray<btCollisionShape*>& collisionShapes, btDiscreteDynamicsWorld* dynamicsWorld)
    {
        btCollisionShape* colShape = new btBoxShape(btVector3(m_scale.x/2, m_scale.y/2, m_scale.z/2));

		collisionShapes.push_back(colShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
        
		groundTransform.setOrigin(btVector3(m_position.x, m_position.y, m_position.z));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
    	body->setFriction(m_friction);
        body->setRestitution(m_restitution);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
    }

    void CollisionBox::generateCubeMesh()
    {
        float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
        };
        std::vector<unsigned int> indices = {
            0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35
        };
        std::vector<Texture> textures;

        std::vector<Vertex> meshVertices;
        for (int i=0; i < 36; i++)
        {
            // i*3 + (0, 1, 2) for (x, y, z)
            int index = i*8;
            Vertex v;
            v.Position = glm::vec3(vertices[index], vertices[index+1], vertices[index+2]);
            v.Normal = glm::vec3(vertices[index+3], vertices[index+4], vertices[index+5]);
            v.TexCoords = glm::vec2(vertices[index+6], vertices[index+7]);

            meshVertices.push_back(v);
        }

        if (m_mesh != nullptr)
            delete m_mesh;
        m_mesh = new Mesh(meshVertices, indices, textures);
    }
}