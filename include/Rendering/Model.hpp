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

namespace Core::Rendering {
class Model {
  public:
    explicit Model(const char* path) { LoadModel(path); }
    void Draw(Shader const &shader);

  private:
    vector<Mesh> mMeshes;
    string mDirectory;

    void LoadModel(string path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

    vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};
}

#endif // ELYS_MODEL_HPP
