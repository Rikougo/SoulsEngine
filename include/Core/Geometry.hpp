//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_GEOMETRY_HPP
#define ELYS_GEOMETRY_HPP

#include <glm/glm.hpp>

using glm::vec3;

namespace Elys::Geometry {
    struct Plan {
        vec3 normal = {0.0f, 1.0f, 0.0f};
        float distance = 0.0f;

        Plan() = default;

        Plan(const glm::vec3 &p1, const glm::vec3 &norm)
            : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) {}

        Plan(const glm::vec4 &xyzw) {
            glm::vec3 abc{xyzw.x, xyzw.y, xyzw.z};
            float magnitude = glm::length(abc);
            abc = glm::normalize(abc);

            normal = abc;
            distance = xyzw.w / magnitude;
        }

        [[nodiscard]] float GetSignedDistance(const vec3 &point) const {
            return glm::dot(normal, point) - distance;
        }
    };

    struct SphericalCoordinates {
        float theta, phi, radius;
    };

    /// \Brief
    /// Convert couple of angles and distance to cartesian coordinates
    /// https://en.wikipedia.org/wiki/Spherical_coordinate_system
    vec3 ToCartesian(float phi, float theta, float radius);

    /// \Brief
    /// Convert spherical coordinates to cartesian coordinates
    /// https://en.wikipedia.org/wiki/Spherical_coordinate_system
    vec3 ToCartesian(SphericalCoordinates coords);
} // namespace Elys::Geometry

#endif // ELYS_GEOMETRY_HPP
