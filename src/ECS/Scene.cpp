//
// Created by Sam on 3/14/2022.
//

#include "ECS/Scene.hpp"

namespace Elys {
    Scene::Scene() {
        mComponentManager.RegisterComponent<Tag>();
        mComponentManager.RegisterComponent<Transform>();
        mComponentManager.RegisterComponent<RigidBody>();
        mComponentManager.RegisterComponent<Texture>();
        mComponentManager.RegisterComponent<Material>();
        mComponentManager.RegisterComponent<Mesh>();
    }

    Entity Scene::CreateEntity(std::string name) {
        auto newID = mEntityManager.CreateEntity();
        auto entity = Entity(this, newID);
        entity.AddComponent(Tag{std::move(name)});
        entity.AddComponent(Transform{});

        mEntities.push_back(entity);
        return entity;
    }

    void Scene::DestroyEntity(Entity const &entity) {
        throw std::runtime_error("Scene::DestroyEntity : Not implemented yet.");
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

    Entity::~Entity() {
        mScene.reset();
    }
}
