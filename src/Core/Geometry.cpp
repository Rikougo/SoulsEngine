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

    void ResetCollisionManifold(CollisionManifold *result) {
        if (result != 0) {
            result->colliding = false;
            result->normal = glm::vec3(0, 0, 1);
            result->depth = FLT_MAX;
            result->contacts.clear();
        }
    }
    bool ClipToPlane(const Plan &plane, const Line &line, glm::vec3 *outPoint) {
        // Ensure that the line and plane intersect
        glm::vec3 ab = line.End() - line.Start();
        float nAB = glm::dot(plane.normal, ab);
        if (CMP(nAB, 0)) {
            return false;
        }
        // Find the time along the line at which it intersects the plane
        float nA = glm::dot(plane.normal, line.Start());
        float t = (plane.distance - nA) / nAB;

        // If the intersection time was valid, return the point at which the line and plane intersect
        if (t >= 0.0f && t <= 1.0f) {
            if (outPoint != 0) {
                *outPoint = line.Start() + ab * t;
            }
            return true;
        }
        return false;
    }

    glm::vec3 MultiplyVector(const glm::vec3 &vec, const glm::mat4 &mat) {
        glm::vec3 result;
        result.x = vec.x * mat[0][0] + vec.y * mat[1][0] +
                   vec.z * mat[2][0] + 0.0f * mat[3][0];
        result.y = vec.x * mat[0][1] + vec.y * mat[1][1] +
                   vec.z * mat[2][1] + 0.0f * mat[3][1];
        result.z = vec.x * mat[0][2] + vec.y * mat[1][2] +
                   vec.z * mat[2][2] + 0.0f * mat[3][2];
        return result;
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