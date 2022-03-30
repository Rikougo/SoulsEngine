//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_SCENE_HPP
#define ELYS_SCENE_HPP

#include <filesystem>
#include <string>
#include <set>
#include <cmath>
#include <utility>
#include <xutility>

#include <ECS/Node.hpp>
#include <ECS/Components.hpp>
#include <ECS/ComponentManager.hpp>
#include <ECS/EntityManager.hpp>
#include <ECS/SystemManager.hpp>

#include <Render/TextureLoader.hpp>
#include <Render/Mesh.hpp>

using std::set;

namespace Elys {
    class Entity;

    class Scene {
      public:
        Scene();

        void OnUpdate(float deltaTime);
        void OnRuntimeUpdate(float deltaTime);

        Entity CreateEntity(std::string name = "Entity");
        void DestroyEntity(Entity const &entity);
        Entity EntityFromNode(Node const &component);

        std::set<Entity>::iterator begin() { return mEntities.begin(); }
        std::set<Entity>::iterator end() { return mEntities.end(); }
        [[nodiscard]] std::set<Entity>::const_iterator begin() const {return mEntities.begin();}
        [[nodiscard]] std::set<Entity>::const_iterator end() const { return mEntities.end(); }
      private:
        std::set<Entity> mEntities;

        ComponentManager mComponentManager;
        EntityManager mEntityManager;
        SystemManager mSystemManager;

        friend class Entity;
      public:
        static Scene FromFile(std::filesystem::path& path);
    };

    class Entity {
      public:
        Entity() = default;
        Entity(Scene* scene, EntityID id) : mScene(scene), mID(id) {};
        // Entity(std::shared_ptr<Scene> &scene, EntityID id) : mScene(scene), mID(id) {};
        Entity(const Entity& other) = default;

        ~Entity() = default;

        template<typename T> T& AddComponent(T value) const {
            return mScene->mComponentManager.AddComponent(mID, value);
        }

        template<typename T> T& GetComponent() const {
            return mScene->mComponentManager.GetComponent<T>(mID);
        }

        template<typename T> void RemoveComponent() const {
            mScene->mComponentManager.RemoveComponent<T>(mID);
        }

        template<typename T> [[nodiscard]] bool HasComponent() const {
            return mScene->mComponentManager.HasComponent<T>(mID);
        }

        Entity Parent() const {
            auto const &node = GetComponent<Node>();

            return mScene->EntityFromNode(*node.Parent());
        }

        vector<Entity> Children() const {
            auto const &node = GetComponent<Node>();
            vector<Entity> children;

            for (auto childNode : node.Children()) {
                children.push_back(mScene->EntityFromNode(*childNode));
            }

            return children;
        }

        [[nodiscard]] EntityID ID() const { return mID; }

        [[nodiscard]] bool IsValid() const { return mScene != nullptr && mID < MAX_ENTITIES; }

        bool operator==(const Entity &other) const {

            return mID == other.mID && mScene == other.mScene;
        }

        bool operator<(const Entity &other) const {
            return mID < other.mID;
        }

        bool operator>(const Entity &other) const {
            return mID > other.mID;
        }

      private:
        EntityID mID{MAX_ENTITIES};
        Scene* mScene = nullptr;
    };
}

#endif // ELYS_SCENE_HPP
