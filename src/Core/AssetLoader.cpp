//
// Created by Sam on 4/27/2022.
//

#include <Core/AssetLoader.hpp>

namespace Elys {
    const std::unordered_map<string, char const*> AssetLoader::gExtensionToDragType = {
        {".png",     AssetLoader::DragDropType::FILE_IMAGE},
        {".jpg",     AssetLoader::DragDropType::FILE_IMAGE},
        {".jpeg",    AssetLoader::DragDropType::FILE_IMAGE},
        {".escene",  AssetLoader::DragDropType::FILE_SCENE}
    };

    std::unordered_map<string, Texture> AssetLoader::gLoadedTextures = std::unordered_map<string, Texture>();
    std::unordered_map<string, Mesh> AssetLoader::gLoadedMeshes = std::unordered_map<string, Mesh>();

    void AssetLoader::Init() {
        ELYS_CORE_INFO("Start loading assets...");
        gLoadedMeshes["Cube"] = Mesh::Cube();
        gLoadedMeshes["Sphere"] = Mesh::Sphere();
        gLoadedMeshes["Plane32x32"] = Mesh::Plane();

        ELYS_CORE_INFO("Assets loaded successfully.");
    }

    Texture &AssetLoader::TextureFromPath(const path &path) {
        if (!gLoadedTextures.contains(path.string())) {
            gLoadedTextures[path.string()] = Texture::FromPath(gAssetPath / path);
            gLoadedTextures[path.string()].mPath = path.string();
        }

        return gLoadedTextures[path.string()];
    }

    Mesh &AssetLoader::MeshFromPath(const path &path) {
        if (gLoadedMeshes.contains(path.string())) {
            return gLoadedMeshes[path.string()];
        }

        Assimp::Importer import;
        const aiScene *scene = import.ReadFile((gAssetPath/path).string(), aiProcess_Triangulate);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            ELYS_CORE_WARN("Assimp error while loading model {0}. Message : {1}", path.string(), import.GetErrorString());
            return gLoadedMeshes["Cube"];
        }

        MeshPartial partial = ProcessAssimpNode(scene->mRootNode, scene);
        Mesh result(partial);
        result.mPath = path.string();

        gLoadedMeshes[path.string()] = result;

        return gLoadedMeshes[path.string()];
    }

    shared_ptr<Scene> AssetLoader::SceneFromPath(const path &path) {
        auto result = std::make_shared<Scene>();

        std::ifstream inputScene(gAssetPath / path);

        if (inputScene.is_open()) {
            string rawEntity;

            while(std::getline(inputScene, rawEntity)) {
                ComponentSerializer::LoadEntity(rawEntity, result);
            }
        } else {
            ELYS_CORE_ERROR("Couldn't find scene file {}", path.string());
        }

        return result;
    }

    std::unordered_map<string, Mesh> &AssetLoader::MeshesMap() { return gLoadedMeshes; }

    std::vector<string> AssetLoader::LoadedMeshesNames() {
        std::vector<string> result(gLoadedMeshes.size());

        std::transform(gLoadedMeshes.begin(), gLoadedMeshes.end(), result.begin(), [](auto pair){ return pair.first;});

        return result;
    }

    void AssetLoader::SerializeScene(shared_ptr<Scene> &scene, path const &path) {
        std::ofstream sceneOutput(gAssetPath / path);

        if (sceneOutput.is_open()) {
            for(auto entityID : (*scene)) {
                sceneOutput << ComponentSerializer::SerializeEntity(scene->EntityFromID(entityID)) << std::endl;
            }
        } else {
            ELYS_CORE_ERROR("Couldn't open {}", path.string());
        }
    }

    MeshPartial AssetLoader::ProcessAssimpNode(aiNode *node, const aiScene *scene) {
        MeshPartial partialMesh;

        // process all the node's meshes (if any)
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            partialMesh.Append(ProcessAssimpMesh(mesh, scene));
        }
        // then do the same for each of its children
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            partialMesh.Append(ProcessAssimpNode(node->mChildren[i], scene));
        }

        return partialMesh;
    }

    MeshPartial AssetLoader::ProcessAssimpMesh(aiMesh *mesh, const aiScene *scene) {
        MeshPartial result;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;

            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;

            if (mesh->mTextureCoords[0]) {
                vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
                vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
            }

            result.mVertices.push_back(vertex);
        }

        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                result.mIndices.push_back(face.mIndices[j]);
        }

        return result;
    }

    template <>
    MeshRenderer ComponentSerializer::ParseComponent<MeshRenderer>(std::unordered_map<string, string> raw) {
        MeshRenderer result;

        result.mesh = AssetLoader::MeshFromPath(raw.contains("Mesh") ? raw["Mesh"] : "Cube");
        if (raw.contains("UVTiling")) result.material.tiling     = ParseVec2(raw["UVTiling"]);
        if (raw.contains("Texture")) result.material.texture     = AssetLoader::TextureFromPath(raw["Texture"]);
        if (raw.contains("NormalMap")) result.material.normalMap = AssetLoader::TextureFromPath(raw["NormalMap"]);
        if (raw.contains("HeightMap")) result.material.heightMap = AssetLoader::TextureFromPath(raw["HeightMap"]);
        if (raw.contains("Metallic")) result.material.metallic   = std::stof(raw["Metallic"]);
        if (raw.contains("Roughness")) result.material.roughness = std::stof(raw["Roughness"]);
        if (raw.contains("Color")) result.material.albedo        = ParseVec4(raw["Color"]);
        if (raw.contains("Unshaded")) result.material.shaded     = false;

        return result;
    }

    template<>
    Light ComponentSerializer::ParseComponent<Light>(std::unordered_map<string, string> raw) {
        Light result;

        result.intensity = raw.contains("Intensity") ? std::stof(raw["Intensity"]) : 200.0f;
        if (raw.contains("Color")) result.color = ParseVec3(raw["Color"]);

        return result;
    }

    template<>
    string ComponentSerializer::SerializeComponent<MeshRenderer>(MeshRenderer data) {
        std::stringstream ss;

        ss << "MeshRenderer+Mesh=" << data.mesh.Path() << "+UVTiling=" << data.material.tiling.x << "," << data.material.tiling.y;
        ss << "+Color=" << std::to_string(data.material.albedo.x) << "," << std::to_string(data.material.albedo.y) << "," << std::to_string(data.material.albedo.z) << "," << std::to_string(data.material.albedo.w);
        if (!data.material.shaded) ss << "+Unshaded";

        if (data.material.texture) ss << "+Texture=" << data.material.texture->GetPath().string();
        if (data.material.normalMap) ss << "+NormalMap=" << data.material.normalMap->GetPath().string();
        if (data.material.heightMap) ss << "+HeightMap=" << data.material.heightMap->GetPath().string();

        ss << "+Metallic=" << std::to_string(data.material.metallic) << "+Roughness=" << std::to_string(data.material.roughness);

        return ss.str();
    }

    template<>
    string ComponentSerializer::SerializeComponent<Light>(Light data) {
        std::stringstream ss;

        ss << "Light+Intensity=" << std::to_string(data.intensity) << "+Color=" << std::to_string(data.color.r) << "," << std::to_string(data.color.g) << "," << std::to_string(data.color.b);

        return ss.str();
    }

    Entity ComponentSerializer::LoadEntity(const string& raw, shared_ptr<Scene> &sceneRef) {
        std::istringstream rawStream(raw);

        string ID, name, parentID, rawPosition, rawRotation, rawScale;

        std::getline(rawStream, ID, ';');
        std::getline(rawStream, name, ';');
        std::getline(rawStream, parentID, ';');
        std::getline(rawStream, rawPosition, ';');
        std::getline(rawStream, rawRotation, ';');
        std::getline(rawStream, rawScale, ';');

        ELYS_CORE_INFO("Read {} {} {} {} {} {}", ID, name, parentID, rawPosition, rawRotation, rawScale);

        auto entity = sceneRef->CreateEntity(name);
        entity.GetComponent<Node>().SetPosition(ParseVec3(rawPosition));
        entity.GetComponent<Node>().SetRotation(ParseVec3(rawRotation));
        entity.GetComponent<Node>().SetScale(ParseVec3(rawScale));

        string currentRawComponent;
        while(std::getline(rawStream, currentRawComponent, ';')) {
            std::istringstream rawComponentStream(currentRawComponent);
            string componentType, currentRawValue;

            // process component type
            std::getline(rawComponentStream, componentType, '+');

            ELYS_CORE_INFO("New component read {}", componentType);

            // process values
            std::unordered_map<string, string> rawComponentsValue;
            while(std::getline(rawComponentStream, currentRawValue, '+')) {
                string token = currentRawValue.substr(0, currentRawValue.find('='));
                string value = currentRawValue.substr(currentRawValue.find('=') + 1, currentRawValue.length() - (token.length() + 1));
                rawComponentsValue[token] = value;
                ELYS_CORE_INFO("\t- Value: {} = {}", token, value);
            }

            if (componentType == "MeshRenderer") {
                MeshRenderer component = ParseComponent<MeshRenderer>(rawComponentsValue);
                entity.AddComponent(component);
            }
            else if (componentType == "Light") {
                Light component = ParseComponent<Light>(rawComponentsValue);
                entity.AddComponent(component);
            }
            else { ELYS_CORE_WARN("Unknown component type attached to {}[{}] : {}", name, ID, componentType); }
        }

        return entity;
    }

    string ComponentSerializer::SerializeEntity(Entity entity) {
        std::stringstream ss;
        auto node = entity.GetComponent<Node>();

        ss << entity.ID() << ";" << node.name << ";" << entity.Parent().ID() << ";"
           << std::to_string(node.LocalPosition().x) << "," << std::to_string(node.LocalPosition().y) << "," << std::to_string(node.LocalPosition().z) << ";"
           << std::to_string(eulerAngles(node.LocalRotation()).x) << "," << std::to_string(eulerAngles(node.LocalRotation()).y) << "," << std::to_string(eulerAngles(node.LocalRotation()).z) << ";"
           << std::to_string(node.LocalScale().x) << "," << std::to_string(node.LocalScale().y) << "," << std::to_string(node.LocalScale().z) << ";";

        if (entity.HasComponent<MeshRenderer>()) ss << SerializeComponent(entity.GetComponent<MeshRenderer>()) << ";";
        if (entity.HasComponent<Light>()) ss << SerializeComponent(entity.GetComponent<Light>()) << ";";

        return ss.str();
    }

    glm::vec2 ComponentSerializer::ParseVec2(string raw) {
        glm::vec2 result{0.0f};
        for(int i = 0; i < 2; i++) {
            size_t pos = raw.find(',');
            if (pos == std::string::npos && i == 0) {
                ELYS_CORE_ERROR("Error reading vec2, bad input value. {}", raw);
                return {0, 0};
            }

            string value = raw.substr(0, pos);
            result[i] = std::stof(value);
            raw.erase(0, pos + 1);
        }

        return result;
    }
    glm::vec3 ComponentSerializer::ParseVec3(string raw) {
        glm::vec3 result{0.0f};
        for(int i = 0; i < 3; i++) {
            size_t pos = raw.find(',');
            if (pos == std::string::npos && i < 2) {
                ELYS_CORE_ERROR("Error reading vec3, bad input value. {}", raw);
                return {0, 0, 0};
            }

            string value = raw.substr(0, pos);
            result[i] = std::stof(value);
            raw.erase(0, pos + 1);
        }

        return result;
    }
    glm::vec4 ComponentSerializer::ParseVec4(string raw) {
        glm::vec4 result{0.0f};
        for(int i = 0; i < 4; i++) {
            size_t pos = raw.find(',');
            if (pos == std::string::npos && i < 3) {
                ELYS_CORE_ERROR("Error reading vec3, bad input value. {}", raw);
                return {0, 0, 0, 0};
            }

            string value = raw.substr(0, pos);
            result[i] = std::stof(value);
            raw.erase(0, pos + 1);
        }

        return result;
    }
}