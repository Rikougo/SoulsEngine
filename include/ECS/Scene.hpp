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

#include <ECS/Components.hpp>
#include <ECS/ComponentManager.hpp>
#include <ECS/EntityManager.hpp>

#include <Render/TextureLoader.hpp>
#include <Render/MeshLoader.hpp>

using std::set;

namespace Elys {
    class Entity;

    class Scene {
      public:
        Scene();

        void OnUpdate(float deltaTime);
        void OnRuntimeUpdate(float deltaTime);

        [[nodiscard]] Entity CreateEntity(std::string name = "Entity");
        void DestroyEntity(Entity const &entity);

        template <typename T> void RegisterComponent() {
            mComponentManager.RegisterComponent<T>();
        }

        [[nodiscard]] std::_Vector_const_iterator<std::_Vector_val<std::_Simple_types<Entity>>> begin() const {return mEntities.begin();}
        [[nodiscard]] std::_Vector_const_iterator<std::_Vector_val<std::_Simple_types<Entity>>> end() const { return mEntities.end(); }
      private:
        std::vector<Entity> mEntities;

        ComponentManager mComponentManager;
        EntityManager mEntityManager;

        friend class Entity;
      public:
        static Scene FromFile(std::filesystem::path& path);
    };

    class Entity {
      public:
        Entity() = default;
        Entity(Scene* scene, EntityID id) : mScene(scene), mID(id) {};
        Entity(std::shared_ptr<Scene> &scene, EntityID id) : mScene(scene), mID(id) {};
        Entity(const Entity& other) = default;

        ~Entity();

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

        [[nodiscard]] EntityID ID() const { return mID; }

        [[nodiscard]] bool IsValid() const { return mScene != nullptr; }

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
        EntityID mID;
        std::shared_ptr<Scene> mScene = nullptr;
    };
}

#endif // ELYS_SCENE_HPP
