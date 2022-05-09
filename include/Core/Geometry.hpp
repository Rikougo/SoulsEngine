//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_GEOMETRY_HPP
#define ELYS_GEOMETRY_HPP

#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

#define CMP(x, y) (fabsf((x) - (y)) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

namespace Elys::Geometry {
    struct Plan {
        glm::vec3 normal = {0.0f, 1.0f, 0.0f};
        float distance = 0.0f;

        Plan() = default;
        inline Plan(const glm::vec3& n, float d) : normal(n), distance(d) { }

        Plan(const glm::vec3 &p1, const glm::vec3 &norm)
            : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) {}

        Plan(const glm::vec4 &xyzw) {
            glm::vec3 abc{xyzw.x, xyzw.y, xyzw.z};
            float magnitude = glm::length(abc);
            abc = glm::normalize(abc);

            normal = abc;
            distance = xyzw.w / magnitude;
        }

        [[nodiscard]] float GetSignedDistance(const glm::vec3 &point) const {
            return glm::dot(normal, point) - distance;
        }
    };

    class Line {
      private:
        glm::vec3 mStart{0}, mEnd{0};
      public:
        Line() = default;
        Line(glm::vec3 start, glm::vec3 end);

        [[nodiscard]] glm::vec3 Start() const;
        [[nodiscard]] glm::vec3 End() const;

        float Length() const;
    };

    class Ray {
      private:
        glm::vec3 mOrigin{0}, mDirection{0};
      public:
        Ray() = default;
        Ray(glm::vec3 origin, glm::vec3 direction);

        [[nodiscard]] glm::vec3 Origin() const;
        [[nodiscard]] glm::vec3 Direction() const;

        void SetDirection(glm::vec3 direction);
    };

    struct RaycastResult {
        glm::vec3 point{0.0f};
        glm::vec3 normal{0.0f};
        float t{0.0f};
        bool hit{false};
    };

    struct CollisionManifold {
        bool colliding;
        glm::vec3 normal;
        float depth;
        std::vector<glm::vec3> contacts;
    };
    void ResetCollisionManifold(CollisionManifold* result);


    struct SphericalCoordinates {
        float theta, phi, radius;
    };

    /// \Brief
    /// Convert couple of angles and distance to cartesian coordinates
    /// https://en.wikipedia.org/wiki/Spherical_coordinate_system
    glm::vec3 ToCartesian(float phi, float theta, float radius);

    /// \Brief
    /// Convert spherical coordinates to cartesian coordinates
    /// https://en.wikipedia.org/wiki/Spherical_coordinate_system
    glm::vec3 ToCartesian(SphericalCoordinates coords);

    bool ClipToPlane(const Plan& plane, const Line& line, glm::vec3* outPoint);

} // namespace Elys::Geometry

#endif // ELYS_GEOMETRY_HPP
