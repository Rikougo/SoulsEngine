//
// Created by Sam on 3/30/2022.
//

#include <Core/Geometry.hpp>

namespace Elys::Geometry {
    glm::vec3 ToCartesian(float phi, float theta, float radius) {
        float x = radius * sin(phi) * sin(theta);
        float y = radius * cos(phi);
        float z = radius * sin(phi) * cos(theta);

        return {x, y, z};
    }

    glm::vec3 ToCartesian(SphericalCoordinates coords)  {
        return ToCartesian(coords.phi, coords.theta, coords.radius);
    }
}