//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_APPLICATION_HPP
#define ELYS_APPLICATION_HPP

#include <functional>
#include <memory>

#include <Managers/ComponentManager.hpp>
#include <Managers/EntityManager.hpp>
#include <Managers/EventManager.hpp>
#include <Managers/SystemManager.hpp>

using namespace std;

namespace Core {
class Application {
  private:
    unique_ptr<ComponentManager> mComponentManager;
    unique_ptr<EntityManager> mEntityManager;
    unique_ptr<SystemManager> mSystemManager;
    unique_ptr<EventManager> mEventManager;

  public:
    Application();

    void AddEventListener(EventId eventId,
                          std::function<void(Event &)> const &listener);
    void DispatchEvent(EventId eventId);
    void DispatchEvent(Event &event);

    // Entity methods
    EntityID CreateEntity() { return mEntityManager->CreateEntity(); }

    void DestroyEntity(EntityID entity) {
        mEntityManager->DestroyEntity(entity);

        mComponentManager->EntityDestroyed(entity);

        mSystemManager->EntityDestroyed(entity);
    }

    // Component methods
    template <typename T> void RegisterComponent() {
        mComponentManager->RegisterComponent<T>();
    }

    template <typename T> void AddComponent(EntityID entity, T& component) {
        mComponentManager->AddComponent<T>(entity, component);

        auto signature = mEntityManager->GetSignature(entity);
        signature.set(mComponentManager->GetComponentType<T>(), true);
        mEntityManager->SetSignature(entity, signature);

        mSystemManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T> void RemoveComponent(EntityID entity) {
        mComponentManager->RemoveComponent<T>(entity);

        auto signature = mEntityManager->GetSignature(entity);
        signature.set(mComponentManager->GetComponentType<T>(), false);
        mEntityManager->SetSignature(entity, signature);

        mSystemManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T> T &GetComponent(EntityID entity) {
        return mComponentManager->GetComponent<T>(entity);
    }

    template <typename T> ComponentType GetComponentType() {
        return mComponentManager->GetComponentType<T>();
    }

    // System methods
    template <typename T> std::shared_ptr<T> RegisterSystem() {
        return mSystemManager->RegisterSystem<T>();
    }

    template <typename T> void SetSystemSignature(Signature signature) {
        mSystemManager->SetSignature<T>(signature);
    }
};
} // namespace Core

#endif // ELYS_APPLICATION_HPP
