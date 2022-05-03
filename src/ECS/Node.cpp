//
// Created by Sam on 3/24/2022.
//

#include "ECS/Components/Node.hpp"

namespace Elys {

    Node::Node()
        : mParent(nullptr),
          mLocalPosition(0.0f, 0.0f, 0.0f),
          mLocalRotation(vec3(0.0f, 0.0f, 0.0f)),
          mLocalScale(1.0f, 1.0f, 1.0f)
    {}

    void Node::SetParent(Node *parent) {
        if (parent == this) {
            ELYS_CORE_WARN("Can't add self as parent !");
            return;
        }

        if (mParent) {
            mParent->RemoveChild(this);
        }

        if (parent) {
            mParent = parent;
            mParent->AddChild(this);
        } else {
            mParent = nullptr;
        }
    }
    void Node::AddChild(Node *child) {
        if (std::find(mChildren.begin(), mChildren.end(), child) != mChildren.end()) {
            ELYS_CORE_WARN("Child node is already a child of parent node.");
            return;
        }

        if (child == this) {
            ELYS_CORE_WARN("Can't put self as child.");
            return;
        }

        mChildren.push_back(child);
        child->mParent = this;
    }
    void Node::RemoveChild(Node *node) {
        auto it = std::find(mChildren.begin(), mChildren.end(), node);
        if (it != mChildren.end()) {
            (*it)->mParent = nullptr;
            mChildren.erase(it);
        }
        else ELYS_CORE_WARN("Child not found.");
    }

    Node* Node::Parent() const { return mParent; }
    vector<Node *> Node::Children() const { return mChildren; }

    void Node::OnDelete() {
        if (mParent) mParent->RemoveChild(this);

        // set new parent the parent of deleted node
        // no matter if it hasn't any parent (children will have no parent)
        for (auto child : mChildren) {
            child->SetParent(mParent);
        }
    };

    vec3 Node::InheritedPosition() const {
        if (!mGlobalUpdated)
            UpdateTransform();
        return mGlobalPosition;
    }
    vec3 Node::InheritedScale() const {
        if (!mGlobalUpdated)
            UpdateTransform();
        return mGlobalScale;
    }
    quat Node::InheritedRotation() const {
        if (!mGlobalUpdated)
            UpdateTransform();
        return mGlobalRotation;
    }
    mat4 Node::InheritedTransform() const {
        if (!mGlobalUpdated)
            UpdateTransform();

        return mGlobalTransform;
    }

    bool Node::InheritedEnabled() const {
        if (mParent) return mParent->LocalEnabled() && mLocalEnabled;
        else return mLocalEnabled;
    }

    vec3 Node::LocalPosition() const { return mLocalPosition; }
    vec3 Node::LocalScale() const { return mLocalScale; }
    quat Node::LocalRotation() const { return mLocalRotation; }
    mat4 Node::LocalTransform() const { return mLocalTransform; }
    bool Node::LocalEnabled() const { return mLocalEnabled; }

    void Node::Move(vec3 translation) {
        mLocalPosition += translation;
        InvalidateNode();
    }
    void Node::Rotate(quat rotation) {
        mLocalRotation *= rotation;
        InvalidateNode();
    }
    void Node::Scale(vec3 scale) { mLocalScale *= scale; InvalidateNode(); }

    void Node::SetPosition(vec3 position) {
        if (position != mLocalPosition) {
            InvalidateNode();
            mLocalPosition = position;
        }
    }
    void Node::SetPosition(float x, float y, float z) { SetPosition(vec3(x, y, z)); }

    void Node::SetRotation(quat rotation) {
        if (mLocalRotation != rotation) {
            InvalidateNode();
            mLocalRotation = rotation;
        }
    }
    void Node::SetRotation(vec3 eulerAngles) { SetRotation(quat(eulerAngles)); }
    void Node::SetRotation(float eulerX, float eulerY, float eulerZ) { SetRotation(vec3(eulerX, eulerY ,eulerZ)); }

    void Node::SetScale(vec3 scale) {
        if (mLocalScale != scale) {
            mLocalScale = scale;
            InvalidateNode();
        }
    }
    void Node::SetScale(float x, float y, float z) { SetScale(vec3(x, y, z)); }
    void Node::SetScale(float uniformScale) { SetScale(vec3(uniformScale, uniformScale, uniformScale)); }

    void Node::SetEnabled(bool enabled) { mLocalEnabled = enabled; }

    void Node::InvalidateNode() const {
        for(auto child : mChildren) child->InvalidateNode();

        mGlobalUpdated = false;
    }

    void Node::UpdateTransform() const {
        if (mParent) {
            if (!mParent->mGlobalUpdated) mParent->UpdateTransform();

            mGlobalPosition = mParent->mGlobalRotation * (mParent->mGlobalScale * mLocalPosition) + mParent->mGlobalPosition;
            mGlobalRotation = mParent->mGlobalRotation * mLocalRotation;
            mGlobalScale    = mParent->mGlobalScale * mLocalScale;
        } else {
            mGlobalPosition = mLocalPosition;
            mGlobalRotation = mLocalRotation;
            mGlobalScale    = mLocalScale;
        }

        mGlobalTransform = mat4(1.0f);
        mGlobalTransform = glm::translate(mGlobalTransform, mGlobalPosition);
        mGlobalTransform *= glm::mat4_cast(mGlobalRotation);
        mGlobalTransform = glm::scale(mGlobalTransform, mGlobalScale);

        mLocalTransform = mat4(1.0f);
        mLocalTransform = glm::translate(mLocalTransform, mLocalPosition);
        mLocalTransform *= glm::mat4_cast(mLocalRotation);
        mLocalTransform = glm::translate(mLocalTransform, mLocalScale);

        mGlobalUpdated = true;
    }
} // namespace Elys