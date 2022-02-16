//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_SYSTEMMANAGER_HPP
#define ELYS_SYSTEMMANAGER_HPP

#include <memory>

#include <ECS/System.hpp>
#include <Utils.hpp>

namespace Core {
class SystemManager {
  public:
    template <typename T> std::shared_ptr<T> RegisterSystem() {
        const char *typeName = typeid(T).name();

        if (mSystems.find(typeName) != mSystems.end())
            throw std::runtime_error("Core::SystemManager::RegisterSystem<T> : "
                                     "System already registered.");

        // Create a pointer to the system and return it so it can be used
        // externally
        auto system = std::make_shared<T>();
        mSystems.insert({typeName, system});
        return system;
    }

    template <typename T> void SetSignature(Signature signature) {
        const char *typeName = typeid(T).name();

        if (mSystems.find(typeName) == mSystems.end())
            throw std::runtime_error("Core::SystemManager::SetSignature<T> : "
                                     "System is not registered.");

        // Set the signature for this system
        mSignatures.insert({typeName, signature});
    }

    void EntityDestroyed(EntityID entity) {
        // Erase a destroyed entity from all system lists
        // mEntities is a set so no check needed
        for (auto const &pair : mSystems) {
            auto const &system = pair.second;

            system->mEntities.erase(entity);
        }
    }

    void EntitySignatureChanged(EntityID entity, Signature entitySignature) {
        // Notify each system that an entity's signature changed
        for (auto const &pair : mSystems) {
            auto const &type = pair.first;
            auto const &system = pair.second;
            auto const &systemSignature = mSignatures[type];

            // Entity signature matches system signature - insert into set
            if ((entitySignature & systemSignature) == systemSignature) {
                system->mEntities.insert(entity);
            }
            // Entity signature does not match system signature - erase from set
            else {
                system->mEntities.erase(entity);
            }
        }
    }

  private:
    std::unordered_map<const char *, Signature> mSignatures{};
    std::unordered_map<const char *, std::shared_ptr<System>> mSystems{};
};
} // namespace Core

#endif // ELYS_SYSTEMMANAGER_HPP
