//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_COMPONENTS_HPP
#define ELYS_COMPONENTS_HPP

#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <Core/Logger.hpp>

using std::string;

using glm::vec3;
using glm::mat4;

namespace Elys {
    struct Tag {
        string name;
    };

    /*class Transform {
      public:
        Transform() = default;
        Transform(vec3 pos, vec3 rot, vec3 sca) : position(pos), rotation(rot), scale(sca) {};

        vec3 position{0.0f, 0.0f, 0.0f};
        vec3 scale{1.0f, 1.0f, 1.0f};
        vec3 rotation{0.0f, 0.0f, 0.0f};
      private:
        vec3 mOldPosition = position;
        vec3 mOldScale = scale;
        vec3 mOldRotation = rotation;

        mat4 mModelMatrix{1.0f};
        // bool mDirty = true;
      public:
        mat4 ModelMatrix() {
            if (mOldPosition != position || mOldRotation != rotation || mOldScale != scale) {
                mModelMatrix = mat4{1.0f};
                mModelMatrix = glm::translate(mModelMatrix, position);

                for(uint8_t i = 0; i < 3; i++) {
                    vec3 axis{0.0f};
                    axis[i] = 1.0f;
                    mModelMatrix = glm::rotate(mModelMatrix, glm::radians(rotation[i]), axis);
                }

                mModelMatrix = glm::scale(mModelMatrix, scale);

                mOldPosition = position;
                mOldRotation = rotation;
                mOldScale = scale;
            }

            return mModelMatrix;
        }
    };*/

    struct RigidBody {
        vec3 velocity;
        float mass;
    };

    struct Material {
        vec3 diffuse;
        vec3 specular;
        vec3 ambient{0.1f, 0.1f, 0.1f};
    };
}

#endif // ELYS_COMPONENTS_HPP
