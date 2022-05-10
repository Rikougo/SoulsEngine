//
// Created by sakeiru on 5/3/2022.
//

#include "ECS/Systems/PlayerSystem.hpp"

namespace Elys {
    void Elys::PlayerSystem::Update(float deltaTime) {
        using glm::vec3;

        for(auto id : mEntities) {
            auto entity = mCurrentScene->EntityFromID(id);
            auto const &node = entity.GetComponent<Node>();
            auto &rBody = entity.GetComponent<RigidBody>();

            glm::vec3 velocity{0.0f};

            if (Input::IsKeyPressed(Key::D)) {
                velocity.x -= 1.0f;
            }

            if (Input::IsKeyPressed(Key::A)) {
                velocity.x += 1.0f;
            }

            if (Input::IsKeyPressed(Key::S)) {
                velocity.z -= 1.0f;
            }

            if (Input::IsKeyPressed(Key::W)) {
                velocity.z += 1.0f;
            }

            rBody.AddLinearImpulse(velocity * entity.GetComponent<Player>().speed * deltaTime);

            mPlayerCamera->SetPosition(node.InheritedPosition() + vec3{0.0f, 4.0f, -6.0f});
            mPlayerCamera->SetTarget(node.InheritedPosition() + vec3{0.0f, node.InheritedScale().y / 2.0f, 0.0f});
        }
    }
}