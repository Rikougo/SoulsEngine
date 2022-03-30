//
// Created by Sam on 3/24/2022.
//

#include <ECS/Node.hpp>

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

        mParent = parent;

        if (mParent) {
            mParent->AddChild(this);
        }
    }
    void Node::AddChild(Node *child) {
        if (std::find(mChildren.begin(), mChildren.end(), child) != mChildren.end()) {
            ELYS_CORE_WARN("Child node is already a child of parent node.");
            return;
        }

        mChildren.push_back(child);
        child->mParent = this;
    }
    void Node::RemoveChild(Node *node) {
        auto it = std::find(mChildren.begin(), mChildren.end(), node);
        if (it != mChildren.end()) mChildren.erase(it);
        else ELYS_CORE_WARN("Child not found.");
    }

    Node* Node::Parent() const { return mParent; }
    vector<Node *> Node::Children() const { return mChildren; }

    vec3 Node::InheritedPosition() const { if (!mGlobalUpdated) UpdateTransform(); return mGlobalPosition; }
    vec3 Node::InheritedScale() const { if (!mGlobalUpdated) UpdateTransform(); return mGlobalScale; }
    quat Node::InheritedRotation() const { if (!mGlobalUpdated) UpdateTransform(); return mGlobalRotation; }
    mat4 Node::InheritedTransform() const {
        if (!mGlobalUpdated) UpdateTransform();

        mat4 model(1.0f);

        model = glm::translate(model, InheritedPosition());

        model *= mat4_cast(InheritedRotation());

        model = glm::scale(model, InheritedScale());

        return model;
    }

    vec3 Node::LocalPosition() const { return mLocalPosition; }
    vec3 Node::LocalScale() const { return mLocalScale; }
    quat Node::LocalRotation() const { return mLocalRotation; }
    mat4 Node::LocalTransform() const {
        mat4 model(1.0f);

        model = glm::translate(model, LocalPosition());

        model *= mat4_cast(LocalRotation());

        model = glm::scale(model, LocalScale());

        return model;
    }

    void Node::Move(vec3 translation) { mLocalPosition += translation; InvalidateNode(); }
    void Node::Rotate(quat rotation) { mLocalRotation *= rotation; InvalidateNode(); }
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
            ELYS_CORE_INFO("SCALE");
            mLocalScale = scale;
            InvalidateNode();
        }
    }
    // void Node::SetScale(float x, float y, float z) { SetScale(vec3(x, y, z)); }
    void Node::SetScale(float uniformScale) { ELYS_CORE_INFO("UNIFORM SCALE"); SetScale(vec3(uniformScale, uniformScale, uniformScale)); }

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

        mGlobalUpdated = true;
    }
} // namespace Elys