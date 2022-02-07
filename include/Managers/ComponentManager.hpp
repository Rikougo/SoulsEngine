//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_COMPONENT_MANAGER_HPP
#define ELYS_COMPONENT_MANAGER_HPP

#include <ECS/ComponentArray.hpp>
#include <Utils.hpp>

namespace Core {
class ComponentManager {
  public:
    template <typename T> void RegisterComponent() {
        const char *typeName = typeid(T).name();

        if (mComponentTypes.find(typeName) != mComponentTypes.end())
            throw std::runtime_error(
                "Core::ComponentManager::RegisterComponent<T> "
                ": Component type already registered");

        mComponentTypes.insert({typeName, mNextComponentType});

        mComponentArrays.insert(
            {typeName, std::make_shared<ComponentArray<T>>()});

        ++mNextComponentType;
    }

    template <typename T> ComponentType GetComponentType() {
        const char *typeName = typeid(T).name();

        if (mComponentTypes.find(typeName) == mComponentTypes.end())
            throw std::runtime_error(
                "Core::ComponentManager::GetComponentType<T> "
                ": Component type is not registered.");

        return mComponentTypes[typeName];
    }

    template <typename T> void AddComponent(Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template <typename T> void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template <typename T> T &GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity) {
        for (auto const &pair : mComponentArrays) {
            auto const &component = pair.second;

            component->EntityDestroyed(entity);
        }
    }

  private:
    std::unordered_map<const char *, ComponentType> mComponentTypes{};
    std::unordered_map<const char *, std::shared_ptr<IComponentArray>>
        mComponentArrays{};

    ComponentType mNextComponentType{};

    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        const char *typeName = typeid(T).name();

        if (mComponentTypes.find(typeName) == mComponentTypes.end())
            throw std::runtime_error(
                "Core::ComponentManager::GetComponentArray<T> : Component not "
                "registered.");

        return std::static_pointer_cast<ComponentArray<T>>(
            mComponentArrays[typeName]);
    }
};
} // namespace Core

#endif // ELYS_COMPONENT_MANAGER_HPP