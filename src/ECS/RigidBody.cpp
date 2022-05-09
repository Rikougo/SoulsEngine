//
// Created by pierrhum on 04/05/2022.
//

#include "ECS/Components/RigidBody.hpp"

using glm::vec3;

Elys::RigidBody::RigidBody(const Elys::Mesh &mesh) {
    mBoundingBox = AABB(mesh);
}

Elys::RigidBody::RigidBody(const vec3 &center, const vec3 &size, const glm::mat3 &rotation) {
    mBoundingBox = OBB(center, size, rotation);
}

void Elys::RigidBody::Update(float deltaTime) {
    mOldPosition = mPosition;
    vec3 oldVelocity = mVelocity;

    // The dampening simulates air friction
    const float damping = 0.98f;
    vec3 acceleration = mForces * (1.0f / mass);
    mVelocity = mVelocity * friction + acceleration * deltaTime;
    mVelocity = mVelocity * damping;

    mPosition = mPosition + (oldVelocity + mVelocity) * 0.5f * deltaTime;

    SolveConstraints();

    mConstraints.clear();
}

void Elys::RigidBody::ApplyForces() {
    mForces = useGravity ? vec3{0.0f, -9.81f, 0.0f} * mass : vec3{0.0f};
}

void Elys::RigidBody::AddLinearImpulse(const vec3 impulse) {
    mVelocity = mVelocity + impulse;
}

void Elys::RigidBody::SolveConstraints() {
    auto size = mConstraints.size();

    if (mOldPosition == mPosition) return;

    for(size_t i = 0; i < size; i++) {
        Geometry::Line traveled(mOldPosition, mPosition);
        if (AABB::Linetest(*mConstraints[i], traveled)) {
            glm::vec3 direction = glm::normalize(mVelocity);
            Geometry::Ray ray(mOldPosition, direction);

            Geometry::RaycastResult result;
            if (AABB::Raycast(*mConstraints[i], ray, result)) {
                mPosition = result.point + result.normal * 0.002f;
                glm::vec3 vn = result.normal * dot(result.normal, mVelocity);
                glm::vec3 vt = mVelocity - vn;

                mOldPosition = mPosition;
                mVelocity = vt - vn * bounce;

                break;
            }
        }
    }
}

void Elys::RigidBody::PushConstraints(Elys::AABB* aabb) {
    mConstraints.push_back(aabb);
}

void Elys::RigidBody::SetPosition(vec3 position) {
    mOldPosition = position;
    mPosition = position;
}
void Elys::RigidBody::ResetVelocity() {mVelocity = vec3(0.0f);}

