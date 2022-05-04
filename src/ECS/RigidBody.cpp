//
// Created by pierrhum on 04/05/2022.
//

#include "ECS/Components/RigidBody.hpp"

Elys::RigidBody::RigidBody(const Elys::Mesh &mesh) {
    mBoundingBox = new AABB(mesh);
}

