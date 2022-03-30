//
// Created by Sam on 3/14/2022.
//

#include "ECS/Scene.hpp"

namespace Elys {
    Scene::Scene() {
        mComponentManager.RegisterComponent<Node>();
        mComponentManager.RegisterComponent<Tag>();
        mComponentManager.RegisterComponent<RigidBody>();
        mComponentManager.RegisterComponent<Texture>();
        mComponentManager.RegisterComponent<Mesh>();
    }

    Entity Scene::CreateEntity(std::string name) {
        auto newID = mEntityManager.CreateEntity();
        auto entity = Entity(this, newID);
        entity.AddComponent(Tag{std::move(name)});
        entity.AddComponent(Node{});

        mEntities.insert(entity);

        ELYS_CORE_INFO("Created entity {0}", newID);
        return entity;
    }

    void Scene::DestroyEntity(Entity const &entity) {
        if (!entity.IsValid()) return;

        entity.GetComponent<Node>().OnDelete();
        mEntities.erase(entity);
    }

    Entity Scene::EntityFromNode(const Node &component) {
        auto id = mComponentManager.GetEntity<Node>(component);
        return Entity(this, id);
    }

    void Scene::OnUpdate(float deltaTime) {
        throw std::runtime_error("Scene::OnUpdate : Not implemented yet.");
    }

    void Scene::OnRuntimeUpdate(float deltaTime) {
        throw std::runtime_error("Scene::OnRuntimeUpdate : Not implemented yet.");
    }

    Scene Scene::FromFile(std::filesystem::path &path) {
        throw std::runtime_error("Scene::OnRuntimeUpdate : Not implemented yet.");
    }
}
