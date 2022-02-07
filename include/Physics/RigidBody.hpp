//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_RIGID_BODY_HPP
#define ELYS_RIGID_BODY_HPP

#include <glm/glm.hpp>

using glm::vec3;

namespace Core::Physics {
struct RigidBody {
    vec3 velocity;
};
}

#endif // ELYS_RIGID_BODY_HPP
