//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_PHYSICS_TRANSFORM_HPP
#define ELYS_PHYSICS_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using glm::mat4;
using glm::vec3;

namespace Core::Physics {
constexpr vec3 WORLD_X_AXIS{1.0f, 0.0f, 0.0f};
constexpr vec3 WORLD_Y_AXIS{0.0f, 1.0f, 0.0f};
constexpr vec3 WORLD_Z_AXIS{0.0f, 0.0f, 1.0f};

struct Transform {
    vec3 position{0.0f, 0.0f, 0.0f};
    vec3 rotation{0.0f, 0.0f, 0.0f};
    vec3 scale{1.0f, 1.0f, 1.0f};

    mat4 ModelMatrix() const {
        mat4 model{1.0f};

        model = glm::translate(model, position);

        model = glm::rotate(model, glm::radians(rotation[0]), WORLD_X_AXIS);
        model = glm::rotate(model, glm::radians(rotation[1]), WORLD_Y_AXIS);
        model = glm::rotate(model, glm::radians(rotation[2]), WORLD_Z_AXIS);

        model = glm::scale(model, scale);

        return model;
    }
};
} // namespace Core::Physics

#endif // ELYS_PHYSICS_TRANSFORM_HPP
