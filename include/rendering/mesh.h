#pragma once


#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rendering/shader.h"

namespace Rendering {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };


    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };


    class Mesh {
        public:
            Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
            void draw(Shader *shader);

        private:
            unsigned int m_VAO, m_VBO, m_EBO;

            std::vector<Vertex>        m_vertices;
            std::vector<unsigned int>  m_indices ;
            std::vector<Texture>       m_textures;

            void setupMesh();
    };


}

