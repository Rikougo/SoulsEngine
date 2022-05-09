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

glm::mat4 Elys::RigidBody::InvTensor() {
    float ix = 0.0f;
    float iy = 0.0f;
    float iz = 0.0f;
    float iw = 0.0f;

    if (mass != 0) {
        vec3 size = box.size * 2.0f;
        float fraction = (1.0f / 12.0f);
        float x2 = size.x * size.x;
        float y2 = size.y * size.y;
        float z2 = size.z * size.z;
        ix = (y2 + z2) * mass * fraction;
        iy = (x2 + z2) * mass * fraction;
        iz = (x2 + y2) * mass * fraction;
        iw = 1.0f;
    }

    return Geometry::Matrix::Inverse(glm::mat4(
        ix, 0, 0, 0,
        0, iy, 0, 0,
        0, 0, iz, 0,
        0, 0, 0, iw));

}
void Elys::RigidBody::AddRotationalImpulse(const vec3 &point, const vec3 &impulse) {
    vec3 centerOfMass = mPosition;
    vec3 torque = glm::cross(point - centerOfMass, impulse);
    glm::mat4 tensor = InvTensor();

    vec3 angAccel{0,0,0};
    angAccel.x = torque.x * tensor[0][0] + torque.y * tensor[1][0] +
                 torque.z * tensor[2][0] + 0.0f * tensor[3][0];
    angAccel.y = torque.x * tensor[0][1] + torque.y * tensor[1][1] +
                 torque.z * tensor[2][1] + 0.0f * tensor[3][1];
    angAccel.z = torque.x * tensor[0][2] + torque.y * tensor[1][2] +
                 torque.z * tensor[2][2] + 0.0f * tensor[3][2];

    angVel = angVel + angAccel;
}
