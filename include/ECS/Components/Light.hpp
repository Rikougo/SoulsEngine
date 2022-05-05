//
// Created by Sam on 4/7/2022.
//

#ifndef ELYS_LIGHT_HPP
#define ELYS_LIGHT_HPP

#include <glm/glm.hpp>

struct Light {
    glm::vec3 color = {1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
};

#endif // ELYS_LIGHT_HPP
