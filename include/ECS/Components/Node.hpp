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

namespace Elys {
    /**
     * @brief
     * Core component of all Entity, it is used to build graph hierarchy
     */
    class Node {
      public:
        std::string name = "Entity";
      public:
        Node();
        // Node(Node &node);
        // Node(Node const &node);

        void SetParent(Node* parent);
        void AddChild(Node* child);
        void RemoveChild(Node* node);
        [[nodiscard]] Node* Parent() const;
        [[nodiscard]] std::vector<Node*> Children() const;

        void OnDelete();

        [[nodiscard]] glm::vec3 InheritedPosition() const;
        [[nodiscard]] glm::vec3 InheritedScale() const;
        [[nodiscard]] glm::quat InheritedRotation() const;
        [[nodiscard]] glm::mat4 InheritedTransform() const;
        [[nodiscard]] bool InheritedEnabled() const;
        [[nodiscard]] glm::vec3 LocalPosition() const;
        [[nodiscard]] glm::vec3 LocalScale() const;
        [[nodiscard]] glm::quat LocalRotation() const;
        [[nodiscard]] glm::mat4 LocalTransform() const;
        [[nodiscard]] bool LocalEnabled() const;

        void Move(glm::vec3 translation);
        void Rotate(glm::quat rotation);
        void Scale(glm::vec3 scale);

        void SetPosition(glm::vec3 position);
        void SetPosition(float x, float y, float z);
        void SetRotation(glm::quat rotation);
        void SetRotation(glm::vec3 eulerAngles);
        void SetRotation(float eulerX, float eulerY, float eulerZ);
        void SetScale(glm::vec3 scale);
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
        std::vector<Node*> mChildren;

        bool mLocalEnabled = true;
        glm::vec3 mLocalPosition{0.0f, 0.0f, 0.0f};
        glm::vec3 mLocalScale{1.0f, 1.0f, 1.0f};
        glm::quat mLocalRotation{glm::vec3(0.0f, 0.0f, 0.0f)};
        mutable glm::mat4 mLocalTransform{1.0f};

        mutable glm::vec3 mGlobalPosition{0.0f, 0.0f, 0.0f};
        mutable glm::vec3 mGlobalScale{1.0f, 1.0f, 1.0f};
        mutable glm::quat mGlobalRotation{glm::vec3{0.0f, 0.0f, 0.0f}};
        mutable bool mGlobalUpdated = false;
        mutable glm::mat4 mGlobalTransform{1.0f};
    };
}

#endif // ELYS_NODE_HPP
