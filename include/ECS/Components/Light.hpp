//
// Created by Sam on 4/7/2022.
//

#ifndef ELYS_LIGHT_HPP
#define ELYS_LIGHT_HPP

#include <glm/glm.hpp>

using glm::vec3;

struct Light {
    vec3 color = {1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
};

#endif // ELYS_LIGHT_HPP
