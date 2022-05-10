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
        mComponentManager->RegisterComponent<MeshRenderer>();
        mComponentManager->RegisterComponent<Light>();
        mComponentManager->RegisterComponent<Player>();
        mComponentManager->RegisterComponent<RigidBody>();
    }

    Entity Scene::CreateEntity(std::string name) {
        auto newID = mEntityManager->CreateEntity();
        auto entity = Entity(this, newID);
        entity.AddComponent(Node{}).name = std::move(name);

        mEntities.insert(newID);

        ELYS_CORE_INFO("Created entity {0}", newID);
        return entity;
    }
    Entity Scene::EntityFromNode(const Node &component) {
        auto id = mComponentManager->GetEntity<Node>(component);
        return {this, id};
    }
    Entity Scene::EntityFromID(EntityID id) { return {this, id}; }

    void Scene::PushToDestroyQueue(EntityID id) { mToDestroy.insert(id); }
    void Scene::ProcessDestroyQueue() {
        for (auto id : mToDestroy) {
            if (id == mSelected)
                mSelected = MAX_ENTITIES;
            if (id == mHovered)
                mHovered = MAX_ENTITIES;

            mEntities.erase(id);
            mComponentManager->GetComponent<Node>(id).OnDelete();

            mSystemManager->EntityDestroyed(id);
            mComponentManager->EntityDestroyed(id);
            mEntityManager->DestroyEntity(id);
        }

        mToDestroy.clear();
    }
}
