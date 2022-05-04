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

#include <Core/Base.hpp>

#include "Core/Event.hpp"

// Components
#include <ECS/Components/Node.hpp>
#include <ECS/Components/Light.hpp>
#include <ECS/Components/MeshRenderer.hpp>
#include <ECS/Components/RigidBody.hpp>
#include <ECS/Components/Player.hpp>

// Managers
#include <ECS/ComponentManager.hpp>
#include <ECS/EntityManager.hpp>
#include <ECS/SystemManager.hpp>

using std::set;
using std::unique_ptr;

namespace Elys {
    class Entity;

    class Scene {
      public:
        Scene();

        Entity CreateEntity(std::string name = "Entity");
        Entity EntityFromNode(Node const &component);
        Entity EntityFromID(EntityID id);

        void SetSelected(int id) { mSelected = id; }
        [[nodiscard]] int GetSelected() const { return mSelected; }

        void SetHovered(int id) { mHovered = id; }
        [[nodiscard]] int GetHovered() const { return mHovered; }

        template<typename T, typename ... Args> std::shared_ptr<T> RegisterSystem(Args&& ... args) {
            return mSystemManager->RegisterSystem<T>(std::forward<Args>(args)...);
        }

        template<typename T, typename ... Components> void SetSystemSignature() {
            mSystemManager->SetSignature<T>(mComponentManager->GetSignature<Components...>());

            for(auto entityID : mEntities) {
                mSystemManager->EntitySignatureChanged(entityID, mEntityManager->GetSignature(entityID));
            }
        }

        std::set<EntityID>::iterator begin() { return mEntities.begin(); }
        std::set<EntityID>::iterator end() { return mEntities.end(); }
        [[nodiscard]] std::set<EntityID>::const_iterator begin() const {return mEntities.begin();}
        [[nodiscard]] std::set<EntityID>::const_iterator end() const { return mEntities.end(); }
      private:
        void DestroyEntity(Entity const &entity);
      private:
        std::set<EntityID> mEntities;

        int mSelected {MAX_ENTITIES}, mHovered {MAX_ENTITIES};

        unique_ptr<ComponentManager> mComponentManager;
        unique_ptr<EntityManager> mEntityManager;
        unique_ptr<SystemManager> mSystemManager;

        friend class Entity;
      public:
        static bool SaveInFile(std::filesystem::path& path);
        static Scene FromFile(std::filesystem::path& path);
    };

    class Entity {
      public:
        Entity() = default;
        Entity(Scene* scene, EntityID id) : mScene(scene), mID(id) {};
        Entity(const Entity& other) = default;

        ~Entity() = default;

        template<typename T> T& AddComponent(T value) const {
            T& comp = mScene->mComponentManager->AddComponent(mID, value);

            // CHANGE SIGNATURE OF ENTITY AND UPDATE SYSTEMS
            Signature newSignature = mScene->mEntityManager->GetSignature(mID);
            newSignature.set(mScene->mComponentManager->GetComponentType<T>(), true);
            mScene->mEntityManager->SetSignature(mID, newSignature);
            mScene->mSystemManager->EntitySignatureChanged(mID, newSignature);
            return comp;
        }

        template<typename T> [[nodiscard]] T& GetComponent() const {
            return mScene->mComponentManager->GetComponent<T>(mID);
        }

        template<typename T> void RemoveComponent() const {
            mScene->mComponentManager->RemoveComponent<T>(mID);

            // CHANGE SIGNATURE OF ENTITY AND UPDATE SYSTEMS
            Signature newSignature = mScene->mEntityManager->GetSignature(mID);
            newSignature.set(mScene->mComponentManager->GetComponentType<T>(), false);
            mScene->mEntityManager->SetSignature(mID, newSignature);
            mScene->mSystemManager->EntitySignatureChanged(mID, newSignature);
        }

        template<typename T> [[nodiscard]] bool HasComponent() const {
            return mScene->mComponentManager->HasComponent<T>(mID);
        }

        void SetParent(Entity &parent) {
            auto &node = GetComponent<Node>();
            auto &parentNode = parent.GetComponent<Node>();

            node.SetParent(&parentNode);
        }

        void AddChildren(Entity &child) {
            auto &node = GetComponent<Node>();
            auto &childNode = child.GetComponent<Node>();

            node.AddChild(&childNode);
        }

        [[nodiscard]] Entity Parent() const {
            auto const &node = GetComponent<Node>();

            if (!node.Parent()) return {};

            return mScene->EntityFromNode(*node.Parent());
        }

        [[nodiscard]] vector<Entity> Children() const {
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
