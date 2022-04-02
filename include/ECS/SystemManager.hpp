//
// Created by Sam on 3/24/2022.
//

#ifndef ELYS_SYSTEM_MANAGER_HPP
#define ELYS_SYSTEM_MANAGER_HPP

#include <memory>

#include <Core/Base.hpp>
#include <ECS/System.hpp>

namespace Elys {
    class SystemManager {
      public:
        template <typename T, typename ... Args> std::shared_ptr<T> RegisterSystem(Args&& ... args) {
            const char *typeName = typeid(T).name();

            if (mSystems.find(typeName) != mSystems.end())
                throw std::runtime_error("Core::SystemManager::RegisterSystem<T> : "
                                         "System already registered.");

            // Create a pointer to the system and return it so it can be used
            // externally
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
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

        void DispatchKeyPressed(KeyPressedEvent &event) {
            for (auto const &pair : mSystems) pair.second->OnKeyPressed(event);
        }

        void DispatchKeyReleased(KeyReleasedEvent &event) {
            for (auto const &pair : mSystems) pair.second->OnKeyReleased(event);
        }

        void DispatchMousePressed(MouseButtonPressedEvent &event) {
            for (auto const &pair : mSystems) pair.second->OnMouseButtonPressed(event);
        }

        void DispatchMouseReleased(MouseButtonReleasedEvent &event) {
            for (auto const &pair : mSystems) pair.second->OnMouseButtonReleased(event);
        }

        void DispatchMouseScroll(MouseScrolledEvent &event) {
            for (auto const &pair : mSystems) pair.second->OnMouseScroll(event);
        }

      private:
        std::unordered_map<const char *, Signature> mSignatures{};
        std::unordered_map<const char *, std::shared_ptr<System>> mSystems{};
    };
} // namespace Elys

#endif // ELYS_SYSTEM_MANAGER_HPP
