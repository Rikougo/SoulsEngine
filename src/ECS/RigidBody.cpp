//
// Created by pierrhum on 04/05/2022.
//

#include "ECS/Components/RigidBody.hpp"

Elys::RigidBody::RigidBody(const Elys::Mesh &mesh) {
    mBoundingBox = new AABB(mesh);
}

void Elys::RigidBody::Update(float deltaTime) {

    mVelocity = {0,0,0};
    if(mUseGravity && !mBoundingBox->IsCollided()) {
        float gravity = sqrt(2 * mGravity);

        mVelocity = {0,-gravity * deltaTime,0};
    }
}