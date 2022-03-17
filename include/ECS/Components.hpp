//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_COMPONENTS_HPP
#define ELYS_COMPONENTS_HPP

#include <string>

#include <glm/glm.hpp>

using std::string;
using glm::vec3;

namespace Elys {
    struct Tag {
        string name;
    };

    struct Transform {
        vec3 position{0.0f, 0.0f, 0.0f};
        vec3 scale{1.0f, 1.0f, 1.0f};
        vec3 rotation{0.0f, 0.0f, 0.0f};
    };

    struct RigidBody {
        vec3 velocity;
        float mass;
    };

    struct Texture {
        unsigned int id;
        string path;
    };

    struct Material {
        vec3 diffuse;
        vec3 specular;
        vec3 ambient{0.1f, 0.1f, 0.1f};
    };
}

#endif // ELYS_COMPONENTS_HPP
