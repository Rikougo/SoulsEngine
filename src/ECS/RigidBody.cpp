//
// Created by pierrhum on 04/05/2022.
//

#include "ECS/Components/RigidBody.hpp"

Elys::RigidBody::RigidBody(const Elys::Mesh &mesh) {
    mBoundingBox = new AABB(mesh);
}

void Elys::RigidBody::Update(float deltaTime) {

	float gravity = sqrt(2 * mGravity);

	mVelocity.x = 0.f * deltaTime;
	mVelocity.y = -gravity * deltaTime;
	mVelocity.z = 0.f * deltaTime;
}