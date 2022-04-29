//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_MANAGERS_COMPONENT_MANAGER_HPP
#define ELYS_MANAGERS_COMPONENT_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include <stdexcept>

#include <Core/Base.hpp>
#include <Core/Logger.hpp>

namespace Elys {
    /// \Brief
    ///     Abstract class of component array used in ComponentManager to store template ComponentArray and still have access to virtual
    ///     method EntityDestroy(EntityID) to handle Entity deletion.
    class IComponentArray {
      public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(EntityID entity) noexcept = 0;
    };

    template <typename T> class ComponentArray : public IComponentArray {
      public:
        ComponentArray() : mSize{0} {}

        void InsertData(EntityID entity, T &component) {
            if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
                throw std::runtime_error("Core::ComponentArray<T>::InsertData : "
                                         "Entity already has data attached.");

            size_t newIndex = mSize;
            mEntityToIndexMap[entity] = newIndex;
            mIndexToEntityMap[newIndex] = entity;
            mComponentArray[newIndex] = component;
            ++mSize;
        }

        /// \Brief
        ///     Removing data is not as trivial as it seems, in fact to preserve
        ///     contiguous data we move the last element of the Array to the place of the
        ///     deleted data. So it costs more than inserting data.
        ///
        /// \Examples
        ///      Component array like this : [A, B, C, D, E]. Removing C component will give : [A, B, E, D]
        void RemoveData(EntityID entity) {
            if (!HasData(entity))
                throw std::runtime_error("RemoveData : Entity not in ComponentArray data.");

            size_t removedIndex = mEntityToIndexMap[entity];
            size_t lastIndex = mSize - 1;
            mComponentArray[removedIndex] = mComponentArray[lastIndex];

            EntityID lastIndexEntity = mIndexToEntityMap[lastIndex];
            mEntityToIndexMap[lastIndexEntity] = removedIndex;
            mIndexToEntityMap[removedIndex] = lastIndexEntity;

            mEntityToIndexMap.erase(entity);
            mIndexToEntityMap.erase(lastIndex);

            --mSize;
        }

        /// \Brief
        ///     Get T Component reference of an Entity.
        ///     Throws exception if entity doesn't have T Component attached.
        T &GetData(EntityID entity) {
            if (!HasData(entity))
                throw std::runtime_error("GetData : Entity not in ComponentArray data.");

            return mComponentArray[mEntityToIndexMap[entity]];
        }

        [[nodiscard]] bool HasData(EntityID entity) noexcept {
            return mEntityToIndexMap.find(entity) != mEntityToIndexMap.end();
        }

        void EntityDestroyed(EntityID entity) noexcept override {
            if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
                RemoveData(entity);
            }
        }

        EntityID GetEntity(T const &component) {
            auto it = std::find(mComponentArray.begin(), mComponentArray.end(), component);

            if (it != mComponentArray.end()) {
                return mIndexToEntityMap[std::distance(mComponentArray.begin(), it)];
            }

            throw std::runtime_error("No entity found for this component.");
        }

      private:
        std::array<T, MAX_ENTITIES> mComponentArray;
        std::unordered_map<EntityID, size_t> mEntityToIndexMap;
        std::unordered_map<size_t, EntityID> mIndexToEntityMap;

        size_t mSize;
    };

    class ComponentManager {
      public:
        template <typename T> void RegisterComponent() {
            const char *typeName = typeid(T).name();

            if (mComponentTypes.find(typeName) != mComponentTypes.end())
                throw std::runtime_error(
                    "Core::ComponentManager::RegisterComponent<T> "
                    ": Component type already registered");

            mComponentTypes.insert({typeName, mNextComponentType});

            mComponentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

            ++mNextComponentType;
        }

        template <typename ... Args> Signature GetSignature() {
            Signature signature;
            (signature.set(GetComponentType<Args>()), ...);

            return signature;
        }

        template <typename T> ComponentType GetComponentType() {
            const char *typeName = typeid(T).name();

            if (mComponentTypes.find(typeName) == mComponentTypes.end())
                throw std::runtime_error(
                    "Core::ComponentManager::GetComponentType<T> "
                    ": Component type is not registered.");

            return mComponentTypes[typeName];
        }

        template <typename T> T& AddComponent(EntityID entity, T component) {
            auto compArray = GetComponentArray<T>();

            compArray->InsertData(entity, component);

            return compArray->GetData(entity);
        }

        template <typename T> void RemoveComponent(EntityID entity) {
            GetComponentArray<T>()->RemoveData(entity);
        }

        template <typename T> T &GetComponent(EntityID entity) {
            return GetComponentArray<T>()->GetData(entity);
        }

        template <typename T> EntityID GetEntity(T const &component) {
            return GetComponentArray<T>()->GetEntity(component);
        }

        template <typename T> [[nodiscard]] bool HasComponent(EntityID entity) noexcept {
            return GetComponentArray<T>()->HasData(entity);
        }

        void EntityDestroyed(EntityID entity) {
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

            return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
        }
    };
} // namespace Core

#endif // ELYS_MANAGERS_COMPONENT_MANAGER_HPP
