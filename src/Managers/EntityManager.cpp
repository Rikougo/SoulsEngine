//
// Created by Sam on 2/6/2022.
//

#include "Managers/EntityManager.hpp"

using namespace Core;

EntityManager::EntityManager() : mLivingEntity{0} {
    for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
        mAvailableEntities.push(entity);
    }
}

EntityID EntityManager::CreateEntity() {
    if (mLivingEntity >= MAX_ENTITIES)
        throw std::runtime_error("Core::EntityManager::CreateEntity : Amount "
                                 "of living entities reached the max.");

    EntityID id = mAvailableEntities.front();
    mAvailableEntities.pop();
    ++mLivingEntity;

    return id;
}

void EntityManager::DestroyEntity(EntityID entity) {
    if (entity >= MAX_ENTITIES)
        throw std::runtime_error(
            "Core::EntityManager::DestroyEntity : Trying to destroy Entity "
            "with an id over max entities capacity.");

    mSignatures[entity].reset();

    mAvailableEntities.push(entity);
    --mLivingEntity;
}

void EntityManager::SetSignature(EntityID entity, Signature signature) {
    if (entity >= MAX_ENTITIES)
        throw std::runtime_error("Core::EntityManager::DestroyEntity : Trying "
                                 "to edit signature of an Entity "
                                 "with an id over max entities capacity.");

    mSignatures[entity] = signature;
}

Signature EntityManager::GetSignature(EntityID entity) const {
    if (entity >= MAX_ENTITIES)
        throw std::runtime_error("Core::EntityManager::DestroyEntity : Trying "
                                 "to get signature of an Entity "
                                 "with an id over max entities capacity.");

    return mSignatures[entity];
}
