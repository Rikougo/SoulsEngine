//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_COMPONENT_ARRAY_HPP
#define ELYS_COMPONENT_ARRAY_HPP

#include <Utils.hpp>

namespace Core {
class IComponentArray {
  public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T> class ComponentArray : public IComponentArray {
  public:
    ComponentArray() : mSize{0} {}

    void InsertData(Entity entity, T component) {
        if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
            throw std::runtime_error("Core::ComponentArray<T>::InsertData : "
                                     "Entity already has data attached.");

        size_t newIndex = mSize;
        mEntityToIndexMap[entity] = newIndex;
        mIndexToEntityMap[newIndex] = entity;
        mComponentArray[newIndex] = component;
        ++mSize;
    }

    /*
     * Removing data is not as trivial as it seems, in fact to preserve
     * contiguous data we move the last element of the Array to the place of the
     * deleted data. So it costs more than inserting data.
     *
     * Examples :
     *      - A component array like this : [A, B, C, D, E]
     *      - If we remove the C component the array will look like : [A, B, E,
     * D]
     */
    void RemoveData(Entity entity) {
        if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
            throw std::runtime_error("Core::ComponentArray<T>::RemoveData : "
                                     "Entity not in ComponentArray data.");

        size_t removedIndex = mEntityToIndexMap[entity];
        size_t lastIndex = mSize - 1;
        mComponentArray[removedIndex] = mComponentArray[lastIndex];

        Entity lastIndexEntity = mIndexToEntityMap[lastIndex];
        mEntityToIndexMap[lastIndexEntity] = removedIndex;
        mIndexToEntityMap[removedIndex] = lastIndexEntity;

        mEntityToIndexMap.erase(entity);
        mIndexToEntityMap.erase(lastIndex);

        --mSize;
    }

    T &GetData(Entity entity) {
        if (mEntityToIndexMap.find(entity) == mEntityToIndexMap.end())
            throw std::runtime_error("Core::ComponentArray<T>::GetData : "
                                     "Entity not in ComponentArray data.");

        return mComponentArray[mEntityToIndexMap[entity]];
    }

    void EntityDestroyed(Entity entity) override {
        if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()) {
            // Remove the entity's component if it existed
            RemoveData(entity);
        }
    }

  private:
    std::array<T, MAX_ENTITIES> mComponentArray;
    std::unordered_map<Entity, size_t> mEntityToIndexMap;
    std::unordered_map<size_t, Entity> mIndexToEntityMap;

    size_t mSize;
};
} // namespace Core

#endif // ELYS_COMPONENT_ARRAY_HPP
