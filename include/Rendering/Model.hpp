//
// Created by Sam on 2/3/2022.
//

#ifndef ELYS_MODEL_HPP
#define ELYS_MODEL_HPP

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Rendering/Mesh.hpp"

using std::vector;
using std::string;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

namespace Core::Rendering {
class Model {
  public:
    explicit Model(const char* path) { loadModel(path); }
    void draw(Shader const &shader);

  private:
    vector<Mesh> mMeshes;
    string mDirectory;

    void loadModel(string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};
}

#endif // ELYS_MODEL_HPP
