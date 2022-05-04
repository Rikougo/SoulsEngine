//
// Created by sakeiru on 5/3/2022.
//

#include "ECS/Systems/PlayerSystem.hpp"

namespace Elys {
    void Elys::PlayerSystem::Update(float deltaTime) {
        using glm::vec2;

        for (auto entityID : mEntities) {
            auto entity = mCurrentScene->EntityFromID(entityID);
            auto &node = entity.GetComponent<Node>();
            auto &player = entity.GetComponent<Player>();
            auto &rBody = entity.GetComponent<RigidBody>();

            vec3 velocity{0.0f};

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

            velocity *= player.speed;

            // rBody.ApplyForce(velocity, deltaTime);

            mPlayerCamera->SetTarget(node.InheritedPosition());
        }
    }
}