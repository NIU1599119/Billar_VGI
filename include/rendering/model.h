#pragma once

// basic includes
#include <string>
#include <vector>

// Asset Import includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// rendering
#include "rendering/mesh.h"


namespace Rendering {

    class Model {
    public:
        Model(std::string path) { loadModel(path); };
        void draw(Shader *shader);
    private:
        std::vector<Mesh> m_meshes;
        std::string directory; // ?

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    };
};
