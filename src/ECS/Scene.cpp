//
// Created by Sam on 3/14/2022.
//

#include "ECS/Scene.hpp"

namespace Elys {
    Scene::Scene() {
        mComponentManager.RegisterComponent<Node>();
        mComponentManager.RegisterComponent<Tag>();
        mComponentManager.RegisterComponent<RigidBody>();
        mComponentManager.RegisterComponent<MeshRenderer>();

        mRenderSystem = mSystemManager.RegisterSystem<RenderSystem>();
        mRenderSystem->SetScene(this);
        Signature renderSignature = mComponentManager.GetComponentType<MeshRenderer>() | mComponentManager.GetComponentType<Node>();
        mSystemManager.SetSignature<RenderSystem>(renderSignature);
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

        EntityID id = entity.ID();

        entity.GetComponent<Node>().OnDelete();
        mEntities.erase(entity);

        mComponentManager.EntityDestroyed(id);
        mSystemManager.EntityDestroyed(id);
        mEntityManager.DestroyEntity(id);
    }

    Entity Scene::EntityFromNode(const Node &component) {
        auto id = mComponentManager.GetEntity<Node>(component);
        return Entity(this, id);
    }

    void Scene::OnUpdate(float deltaTime) {
        mRenderSystem->Update(deltaTime);
    }

    void Scene::OnRuntimeUpdate(float deltaTime) {
        mRenderSystem->Update(deltaTime);
    }

    void Scene::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Scene::OnMousePressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(Scene::OnMouseReleased));
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Scene::OnMouseScroll));
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Scene::OnKeyPressed));
        dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(Scene::OnKeyReleased));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Scene::OnWindowResize));
    };
    // EVENTS HANDLING
    bool Scene::OnWindowResize(WindowResizeEvent &event) {
        mRenderSystem->OnViewportChange(event.GetWidth(), event.GetHeight());
        return false;
    }
    bool Scene::OnKeyPressed(KeyPressedEvent &event) {
        mSystemManager.DispatchKeyPressed(event);
        return false;
    }
    bool Scene::OnKeyReleased(KeyReleasedEvent &event) {
        mSystemManager.DispatchKeyReleased(event);
        return false;
    }
    bool Scene::OnMousePressed(MouseButtonPressedEvent &event) {
        mSystemManager.DispatchMousePressed(event);
        return false;
    }
    bool Scene::OnMouseReleased(MouseButtonReleasedEvent &event) {
        mSystemManager.DispatchMouseReleased(event);
        return false;
    }
    bool Scene::OnMouseScroll(MouseScrolledEvent &event) {
        mSystemManager.DispatchMouseScroll(event);
        return false;
    }

    Scene Scene::FromFile(std::filesystem::path &path) {
        throw std::runtime_error("Scene::OnRuntimeUpdate : Not implemented yet.");
    }
}
