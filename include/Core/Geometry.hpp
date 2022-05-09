//
// Created by Sam on 3/29/2022.
//

#ifndef ELYS_GEOMETRY_HPP
#define ELYS_GEOMETRY_HPP

#include <glm/glm.hpp>

namespace Elys::Geometry {
    struct Plan {
        glm::vec3 normal = {0.0f, 1.0f, 0.0f};
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

    struct Matrix {

        #define CMP(x, y) (fabsf((x) - (y)) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

        static void Transpose(const float *srcMat, float *dstMat,
                       int srcRows, int srcCols) {
            for (int i = 0; i < srcRows * srcCols; i++) {
                int row = i / srcRows;
                int col = i % srcRows;
                dstMat[i] = srcMat[srcCols * col + row];
            }
        }

        static glm::mat4 Transpose(const glm::mat4& matrix) {
            glm::mat4 result;
            Transpose(matrix.asArray, result.asArray, 4, 4);
            return result;
        }

        static glm::mat4 Adjugate(const glm::mat4& mat) {
            return Transpose(Cofactor(mat));
        }

        static glm::mat3 Cut(const glm::mat4 &mat, int row, int col) {
            glm::mat3 result;
            int index = 0;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (i == row || j == col) {
                        continue;
                    }
                    int target = index++;
                    int source = 4 * i + j;
                    result.asArray[target] = mat.asArray[source];
                }
            }
            return result;
        }

        static glm::mat3 Minor(const glm::mat3 &mat) {
            glm::mat3 result;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    result[i][j] = Determinant(Cut(mat, i, j));
                }
            }
            return result;
        }

        void Cofactor(float *out, const float *minor, int rows, int cols) {
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    int t = cols * j + i;            // Target index
                    int s = cols * j + i;            // Source index
                    float sign = powf(-1.0f, i + j); // + or –
                    out[t] = minor[s] * sign;
                }
            }
        }

        static glm::mat3 Cofactor(const glm::mat3 &mat) {
            glm::mat3 result;
            Cofactor(result.asArray, Minor(mat).asArray, 3, 3);
            return result;
        }

        static float Determinant(const glm::mat4 &mat) {
            float result = 0.0f;
            glm::mat4 cofactor = Cofactor(mat);
            for (int j = 0; j < 4; ++j) {
                result += glm::mat.asArray[4 * 0 + j] * cofactor[0][j];
            }
            return result;
        }

        static glm::mat4 Inverse(const glm::mat4 &mat) {
            float det = Determinant(mat);
            if (CMP(det, 0.0f)) {
                return glm::mat4();
            }
            return Adjugate(mat) * (1.0f / det);
        }
    }
} // namespace Elys::Geometry

#endif // ELYS_GEOMETRY_HPP
