//
// Created by Sam on 3/14/2022.
//

#include "ECS/Scene.hpp"

namespace Elys {
    Scene::Scene() {
        mEntityManager = std::make_unique<EntityManager>();
        mComponentManager = std::make_unique<ComponentManager>();
        mSystemManager = std::make_unique<SystemManager>();

        mComponentManager->RegisterComponent<Node>();
        mComponentManager->RegisterComponent<Tag>();
        mComponentManager->RegisterComponent<RigidBody>();
        mComponentManager->RegisterComponent<MeshRenderer>();
        mComponentManager->RegisterComponent<Light>();
    }

    Entity Scene::CreateEntity(std::string name) {
        auto newID = mEntityManager->CreateEntity();
        auto entity = Entity(this, newID);
        entity.AddComponent(Tag{std::move(name)});
        entity.AddComponent(Node{});

        mEntities.insert(entity);

        ELYS_CORE_INFO("Created entity {0}", newID);
        return entity;
    }

    void Scene::DestroyEntity(Entity const &entity) {
        if (!entity.IsValid()) return;

        EntityID id = entity.ID();

        entity.GetComponent<Node>().OnDelete();
        mEntities.erase(entity);

        mComponentManager->EntityDestroyed(id);
        mSystemManager->EntityDestroyed(id);
        mEntityManager->DestroyEntity(id);
    }

    void Scene::PushDestroyQueue(Entity entity) { mDestroyQueue.insert(entity); }

    void Scene::ProcessDestroyQueue() {
        for(Entity e : mDestroyQueue) {
            if (mEntities.contains(e)) DestroyEntity(e);
        }

        mDestroyQueue.clear();
    }

    Entity Scene::EntityFromNode(const Node &component) {
        auto id = mComponentManager->GetEntity<Node>(component);
        return {this, id};
    }

    Entity Scene::EntityFromID(EntityID id) { return {this, id}; }

    /*void Scene::OnUpdate(float deltaTime) { }

    void Scene::OnRuntimeUpdate(float deltaTime) { }*/

    bool Scene::SaveInFile(std::filesystem::path &path) { return false; }

    Scene Scene::FromFile(std::filesystem::path &path) {
        throw std::runtime_error("Scene::OnRuntimeUpdate : Not implemented yet.");
    }
}
