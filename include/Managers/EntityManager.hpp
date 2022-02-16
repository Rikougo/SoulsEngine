//
// Created by Sam on 2/6/2022.
//

#ifndef ELYS_ENTITY_MANAGER_HPP
#define ELYS_ENTITY_MANAGER_HPP

#include <array>
#include <queue>

#include <Utils.hpp>

namespace Core {

class EntityManager {
  private:
    std::queue<EntityID> mAvailableEntities{};
    std::array<Signature, MAX_ENTITIES> mSignatures{};
    uint32_t mLivingEntity;
  public:
    EntityManager();

    EntityID CreateEntity();

    void DestroyEntity(EntityID entity);

    void SetSignature(EntityID entity, Signature signature);
    [[nodiscard]] Signature GetSignature(EntityID entity) const;
};
}

#endif // ELYS_ENTITY_MANAGER_HPP
