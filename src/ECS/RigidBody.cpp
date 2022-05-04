//
// Created by pierrhum on 04/05/2022.
//

#include "ECS/Components/RigidBody.hpp"

Elys::RigidBody::RigidBody(const Elys::Mesh &mesh) {
    mBoundingBox = new AABB(mesh);
}

void Elys::RigidBody::Update(float deltaTime) {

    if(mBoundingBox->IsCollided())
        mVelocity = {0,0,0};
    else {
        // GRAVITY
        ApplyForce(mMass * vec3 {0,-mGravity,0}, deltaTime);
        // Force Normal
        // Get Hit Point
        // W * cos(alpha)
    }

    ELYS_CORE_INFO("Velocity: {0}", mVelocity.y);
}
void Elys::RigidBody::ApplyForce(vec3 force, float dt) {
    mVelocity += force * dt;
}
