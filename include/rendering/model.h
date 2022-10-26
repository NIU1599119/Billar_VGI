#pragma once

// basic includes
#include <string>
#include <vector>
#include "debug.h"

// Asset Import includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Material texture loading
#include <stb_image.h>

// rendering
#include "rendering/mesh.h"


namespace Rendering {

    unsigned int textureFromFile(const std::string &file, const std::string &directory, bool gamma = false);

    class Model {
    public:
        Model(const std::string &path, bool gamma = false) : m_gammaCorrection(gamma) { loadModel(path); };
        void draw(Shader *shader);
    private:
        // model data
        std::vector<Mesh> m_meshes;
        std::string m_directory;
        bool m_gammaCorrection;

        std::vector<Texture> m_texturesLoaded;

        void loadModel(const std::string& path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    };
};
