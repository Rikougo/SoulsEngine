//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_PHYSIC_SYSTEM_HPP
#define ELYS_PHYSIC_SYSTEM_HPP

#include <ECS/System.hpp>
#include <Physics/Transform.hpp>
#include <Physics/RigidBody.hpp>
#include <Application.hpp>
#include <Utils.hpp>

namespace Core::Physics {
class PhysicSystem : public System {
  public:
    void Update(std::shared_ptr<Application> &app);
};
}

#endif // ELYS_PHYSIC_SYSTEM_HPP
