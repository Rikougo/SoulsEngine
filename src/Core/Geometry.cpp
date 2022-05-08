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

    // LINE
    Line::Line(glm::vec3 start, glm::vec3 end) : mStart(start), mEnd(end) {}
    glm::vec3 Line::Start() const { return mStart; }
    glm::vec3 Line::End() const { return mEnd; }
    float Line::Length() const { return glm::length(mStart - mEnd); }

    // RAY
    Ray::Ray(glm::vec3 origin, glm::vec3 direction) : mOrigin(origin), mDirection(direction) {}
    glm::vec3 Ray::Origin() const { return mOrigin; }
    glm::vec3 Ray::Direction() const { return mDirection; }
    void Ray::SetDirection(glm::vec3 direction) { mDirection = direction; }
}