//
// Created by Sam on 4/27/2022.
//

#ifndef ELYS_ASSET_LOADER_HPP
#define ELYS_ASSET_LOADER_HPP

#include <filesystem>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ECS/Scene.hpp>

#include <Render/Texture.hpp>
#include <Render/Mesh.hpp>

namespace Elys {
    class AssetLoader {
      public:
        struct DragDropType {
            static constexpr char const* FILE_IMAGE = "ELYS_FILE_IMAGE";
            static constexpr char const* FILE_SCENE = "ELYS_FILE_SCENE";
        };
      public:
        static constexpr char const* gAssetPath = "./assets";
        static const std::unordered_map<std::string, char const*> gExtensionToDragType;
      public:
        static void Init();

        static Texture& TextureFromPath(std::filesystem::path const &path);
        static Mesh& MeshFromPath(std::filesystem::path const &path);
        static std::shared_ptr<Scene> SceneFromPath(std::filesystem::path const &path);
        static std::unordered_map<std::string, Mesh>& MeshesMap();
        static std::vector<std::string> LoadedMeshesNames();

        static void SerializeScene(std::shared_ptr<Scene> &scene, std::filesystem::path const &path);
      private:
        static MeshPartial ProcessAssimpNode(aiNode *node, const aiScene *scene);
        static MeshPartial ProcessAssimpMesh(aiMesh *mesh, const aiScene *scene);
      private:
        static std::unordered_map<std::string, Mesh> gLoadedMeshes;
        static std::unordered_map<std::string, Texture> gLoadedTextures;
    };

    class ComponentSerializer {
      public:
        static Entity LoadEntity(const std::string& raw, std::shared_ptr<Scene> &sceneRef);
        static std::string SerializeEntity(Entity entity);
      private:
        template<typename T> static std::string SerializeComponent(T const &data);
        template<typename T> static T ParseComponent(std::unordered_map<std::string, std::string> const &raw);
      private:
        static glm::vec2 ParseVec2(std::string raw);
        static glm::vec3 ParseVec3(std::string raw);
        static glm::vec4 ParseVec4(std::string raw);
    };
}

#endif // ELYS_ASSET_LOADER_HPP
