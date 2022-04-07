//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_COMPONENTS_HPP
#define ELYS_COMPONENTS_HPP

#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <Core/Logger.hpp>

// INCLUDE ALL COMPONENTS HERE TO REGROUP ALL
#include <ECS/Components/Node.hpp>
#include <ECS/Components/Light.hpp>
#include <ECS/Components/MeshRenderer.hpp>

using std::string;

using glm::vec3;
using glm::mat4;

// TEMP May be deleted soon
namespace Elys {
    struct Tag {
        string name;
    };

    struct RigidBody {
        vec3 velocity;
        float mass;
    };
}

#endif // ELYS_COMPONENTS_HPP
