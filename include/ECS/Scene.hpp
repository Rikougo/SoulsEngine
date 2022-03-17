//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_SCENE_HPP
#define ELYS_SCENE_HPP

#include <filesystem>
#include <string>
#include <set>
#include <cmath>

#include <ECS/Components.hpp>
#include <ECS/ComponentManager.hpp>
#include <ECS/EntityManager.hpp>
#include <utility>
#include <xutility>

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
        std::unordered_map<EntityID, std::vector<Entity>> mChildren;

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

        Entity CreateChild(std::string name = "Entity");

        [[nodiscard]] EntityID ID() const { return mID; }

        [[nodiscard]] bool IsValid() const { return mScene != nullptr; }

        [[nodiscard]] std::vector<Entity> Children() const {
            if (!IsValid())
                ELYS_CORE_FATAL("Scene attached to Entity is null.");
            return mScene->mChildren[mID];
        }

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
