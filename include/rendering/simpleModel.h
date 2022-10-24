#pragma once

#include "mesh.h"
#include "shader.h"
#include "texture.h"

#include "debug.h"

// matrix operations
#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>


namespace RenderingTemp {

    class SimpleModel {
    /*
        This class is temporal, it may be changed to enable model imports
    */
    public:
        SimpleModel() {};
        SimpleModel(Mesh* mesh, Texture* texture = nullptr) : m_mesh(mesh), m_texture(texture) {};

        void setPosition(glm::vec3* newPosition) { m_position = newPosition; };

        void rotate(float angle, glm::vec3 direction) { // angle is in radians
            glm::quat rotation = glm::angleAxis(angle, direction);
            m_orientation = glm::normalize(m_orientation*rotation);
        }; // implement this later

        void setOrientation(float angle, glm::vec3 direction) { // angle is in radians
            m_orientation = glm::normalize(glm::angleAxis(angle, direction));
        }

        void scale(float scale) { m_scaling = glm::vec3(scale); };

        void draw(Shader* shader, glm::mat4 view, glm::mat4 perspective);

        void redraw(Shader* shader);


    private:
        Mesh* m_mesh;
        // ShaderProgram* m_shader;
        Texture* m_texture;

        glm::vec3* m_position;
        glm::quat m_orientation = glm::quat(0.0f, 0.0f, 1.0f, 0.0f);
        // could add a scaling vector later
        glm::vec3 m_scaling = glm::vec3(1.0f);

        // could save the transformation mat4 here too
    };



    //// does not work
    // class MinecraftCube {
    
    // public:
    //     MinecraftCube() {
    //         m_vertices = {
    //             -0.5f, -0.5f, -0.5f,  uDiv, vDiv,
    //             0.5f, -0.5f, -0.5f,  0.0f, vDiv,
    //             0.5f,  0.5f, -0.5f,  0.0f, 0,
    //             0.5f,  0.5f, -0.5f,  0.0f, 0,
    //             -0.5f,  0.5f, -0.5f,  uDiv, 0,
    //             -0.5f, -0.5f, -0.5f,  uDiv, vDiv,

    //             -0.5f, -0.5f,  0.5f,  0.0f, 2*vDiv,
    //             0.5f, -0.5f,  0.5f,  uDiv, 2*vDiv,
    //             0.5f,  0.5f,  0.5f,  uDiv, 3*vDiv,
    //             0.5f,  0.5f,  0.5f,  uDiv, 3*vDiv,
    //             -0.5f,  0.5f,  0.5f,  0.0f, 3*vDiv,
    //             -0.5f, -0.5f,  0.5f,  0.0f, 2*vDiv,

    //             -0.5f,  0.5f,  0.5f,  3*uDiv, 2*vDiv,
    //             -0.5f,  0.5f, -0.5f,  3*uDiv, vDiv,
    //             -0.5f, -0.5f, -0.5f,  4*uDiv, vDiv,
    //             -0.5f, -0.5f, -0.5f,  4*uDiv, vDiv,
    //             -0.5f, -0.5f,  0.5f,  4*uDiv, 2*vDiv,
    //             -0.5f,  0.5f,  0.5f,  3*uDiv, 2*vDiv,

    //             0.5f,  0.5f,  0.5f,  2*uDiv, 2*vDiv,
    //             0.5f,  0.5f, -0.5f,  2*uDiv, vDiv,
    //             0.5f, -0.5f, -0.5f,  uDiv, vDiv,
    //             0.5f, -0.5f, -0.5f,  uDiv, vDiv,
    //             0.5f, -0.5f,  0.5f,  uDiv, 2*vDiv,
    //             0.5f,  0.5f,  0.5f,  2*uDiv, 2*vDiv,

    //             -0.5f, -0.5f, -0.5f,  0.0f, vDiv,
    //             0.5f, -0.5f, -0.5f,  uDiv, vDiv,
    //             0.5f, -0.5f,  0.5f,  uDiv, vDiv*2,
    //             0.5f, -0.5f,  0.5f,  uDiv, vDiv*2,
    //             -0.5f, -0.5f,  0.5f,  0.0f, vDiv*2,
    //             -0.5f, -0.5f, -0.5f,  0.0f, vDiv,

    //             -0.5f,  0.5f, -0.5f,  3*uDiv, vDiv,
    //             0.5f,  0.5f, -0.5f,  2*uDiv, vDiv,
    //             0.5f,  0.5f,  0.5f,  2*uDiv, vDiv*2,
    //             0.5f,  0.5f,  0.5f,  2*uDiv, vDiv*2,
    //             -0.5f,  0.5f,  0.5f,  3*uDiv, vDiv*2,
    //             -0.5f,  0.5f, -0.5f,  3*uDiv, vDiv
    //         };

    //         std::vector<AttributeData> cubeAttributes;
    //         cubeAttributes.push_back(AttributeData(3, 0));
    //         cubeAttributes.push_back(AttributeData(2, (void*)(3*sizeof(float))));

    //         m_mesh.setVertex(&m_vertices[0], 36, 5*sizeof(float), cubeAttributes);
    //         m_mesh.create();

    //         m_textureDir = "textures/minecraft-grass.png";
    //         Texture m_texture(m_textureDir);

    //         if (!m_texture.create())
    //         {
    //             LOG_ERROR("Failed creating the texture at %s\n", m_textureDir);
    //         }

    //         m_cube = new SimpleModel(&m_mesh, &m_texture);
    //     };

    //     void draw(ShaderProgram* shader, glm::mat4 view, glm::mat4 perspective)
    //     {
    //         m_cube->draw(shader, view, perspective);
    //     }

    //     void redraw(ShaderProgram* shader)
    //     {
    //         m_cube->redraw(shader);
    //     }
        
    //     void setPosition(glm::vec3* newPosition) { m_cube->setPosition(newPosition); };
    

    // private:
    //     static constexpr float uDiv = 1.0/4.0;  // constexpr es lo mismo que const pero para float
    //     static constexpr float vDiv = 1.0/3.0;
    //     std::string m_textureDir;

    //     std::vector<float> m_vertices;

    //     Mesh m_mesh;
    //     Texture m_texture;

    //     SimpleModel* m_cube;
    // };
};


