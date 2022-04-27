//
// Created by Sam on 3/24/2022.
//

#ifndef ELYS_NODE_HPP
#define ELYS_NODE_HPP

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

#include "Core/Logger.hpp"

using glm::vec3;
using glm::mat4;
using glm::quat;

using std::vector;

namespace Elys {
    /**
     * @brief
     * Core component of all Entity, it is used to build graph hierarchy
     */
    class Node {
      public:
        Node();
        // Node(Node &node);
        // Node(Node const &node);

        void SetParent(Node* parent);
        void AddChild(Node* child);
        void RemoveChild(Node* node);
        [[nodiscard]] Node* Parent() const;
        [[nodiscard]] vector<Node*> Children() const;

        void OnDelete();

        [[nodiscard]] vec3 InheritedPosition() const;
        [[nodiscard]] vec3 InheritedScale() const;
        [[nodiscard]] quat InheritedRotation() const;
        [[nodiscard]] mat4 InheritedTransform() const;
        [[nodiscard]] bool InheritedEnabled() const;
        [[nodiscard]] vec3 LocalPosition() const;
        [[nodiscard]] vec3 LocalScale() const;
        [[nodiscard]] quat LocalRotation() const;
        [[nodiscard]] mat4 LocalTransform() const;
        [[nodiscard]] bool LocalEnabled() const;

        void Move(vec3 translation);
        void Rotate(quat rotation);
        void Scale(vec3 scale);

        void SetPosition(vec3 position);
        void SetPosition(float x, float y, float z);
        void SetRotation(quat rotation);
        void SetRotation(vec3 eulerAngles);
        void SetRotation(float eulerX, float eulerY, float eulerZ);
        void SetScale(vec3 scale);
        void SetScale(float x, float y, float z);
        void SetScale(float uniformScale);
        void SetEnabled(bool enabled);

        /**
         * @brief Compare Node using pointer address
         * @param other 
         * @return
         */
        bool operator==(const Node &other) {
            return this == &other;
        }
      private:
        void InvalidateNode() const;
        void UpdateTransform() const;
      private:
        Node* mParent = nullptr;
        vector<Node*> mChildren;

        bool mLocalEnabled = true;
        vec3 mLocalPosition{0.0f, 0.0f, 0.0f};
        vec3 mLocalScale{1.0f, 1.0f, 1.0f};
        quat mLocalRotation{vec3(0.0f, 0.0f, 0.0f)};
        mutable mat4 mLocalTransform{1.0f};

        mutable vec3 mGlobalPosition{0.0f, 0.0f, 0.0f};
        mutable vec3 mGlobalScale{1.0f, 1.0f, 1.0f};
        mutable quat mGlobalRotation{vec3{0.0f, 0.0f, 0.0f}};
        mutable bool mGlobalUpdated = false;
        mutable mat4 mGlobalTransform{1.0f};
    };
}

#endif // ELYS_NODE_HPP
