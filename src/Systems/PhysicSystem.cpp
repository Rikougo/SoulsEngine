//
// Created by Sam on 2/7/2022.
//

#include "Systems/PhysicSystem.hpp"

using namespace Core::Physics;

void PhysicSystem::Update(shared_ptr<Application> &app) {
    for(auto const &entity : mEntities) {
        try {
            auto &transform = app->GetComponent<Transform>(entity);
            auto &rigidBody = app->GetComponent<RigidBody>(entity);

            transform.position += rigidBody.velocity * Time::DeltaTime;
        } catch(std::runtime_error &e) {
            std::cerr << "Core::Physics::PhysicSystem::Update : " << e.what() << std::endl;
        }
    }
}
