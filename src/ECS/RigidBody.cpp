//
// Created by pierrhum on 04/05/2022.
//

#include "ECS/Components/RigidBody.hpp"

using glm::vec3;

Elys::RigidBody::RigidBody(const Elys::Mesh &mesh) {
    mBoundingBox = AABB(mesh);
}

void Elys::RigidBody::Update(float deltaTime) {
    vec3 oldVelocity = {mVelocity.x, mVelocity.y, mVelocity.z};
    if(mUseGravity) {
        // GRAVITY
        ApplyForce(mMass * vec3 {0,-mGravity,0}, deltaTime);
        // Force Normal
        if(mBoundingBox.IsCollided()) {
            mVelocity = mBounce ? vec3{0,-0.5 * oldVelocity.y,0} : vec3{0,0,0};
        }
    }

    // ELYS_CORE_INFO("Velocity: {0}", mVelocity.y);
}
void Elys::RigidBody::ApplyForce(vec3 force, float dt) {
    mVelocity += force * dt;
}
