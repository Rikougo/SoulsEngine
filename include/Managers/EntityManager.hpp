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
    std::queue<Entity> mAvailableEntities{};
    std::array<Signature, MAX_ENTITIES> mSignatures{};
    uint32_t mLivingEntity;
  public:
    EntityManager();

    Entity CreateEntity();

    void DestroyEntity(Entity entity);

    void SetSignature(Entity entity, Signature signature);
    [[nodiscard]] Signature GetSignature(Entity entity) const;
};
}

#endif // ELYS_ENTITY_MANAGER_HPP
