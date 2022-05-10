//
// Created by pierrhum on 04/05/2022.
//

#include "ECS/Components/RigidBody.hpp"

using glm::vec3;

Elys::RigidBody::RigidBody(const Elys::Mesh &mesh) { mVolume = AABB(mesh);
}

Elys::RigidBody::RigidBody(const vec3 &center, const vec3 &size, const glm::mat3 &rotation) {
    mVolume = OBB(center, size, rotation);
}

void Elys::RigidBody::Update(float deltaTime) {
    if(!isKinematic) {
        mOldPosition = mPosition;
        vec3 oldVelocity = mVelocity;

        // The dampening simulates air friction
        const float damping = 0.98f;
        vec3 acceleration = mForces * (1.0f / mass);
        mVelocity = (mVelocity + acceleration * deltaTime) * damping;

        vec3 angAccel = Geometry::MultiplyVector(torques, InvTensor());
        angVel = angVel + angAccel * deltaTime;
        angVel = angVel * damping;

        mPosition = mPosition + mVelocity * deltaTime;
        mOrientation = mOrientation + angVel * deltaTime;

        if(mOrientation.x + mOrientation.y + mOrientation.z <= 0.001f) mOrientation = vec3{0};
        ELYS_CORE_INFO("Velocity : {0} {1} {2}", mVelocity.x, mVelocity.y, mVelocity.z);
        ELYS_CORE_INFO("Angular Velocity : {0} {1} {2}", angVel.x, angVel.y, angVel.z);
        ELYS_CORE_INFO("Orientation : {0} {1} {2}", mOrientation.x, mOrientation.y, mOrientation.z);
    }
}

void Elys::RigidBody::ApplyForces() {
    if(!isKinematic) {
        mForces = useGravity ? vec3{0.0f, -9.81f, 0.0f} * mass : vec3{0.0f};
    }
}

void Elys::RigidBody::ApplyImpulse(Elys::RigidBody &other,
                                   const Elys::Geometry::CollisionManifold &M, int c) {
    if(!isKinematic) {
        // Linear Velocity
        float invMass1 = InvMass();
        float invMass2 = other.InvMass();
        float invMassSum = invMass1 + invMass2;
        if (invMassSum == 0.0f) {
            return;
        }

        // Store the inverse inertia tensor for both the colliding objects
        vec3 r1 = M.contacts[c] - mPosition;
        vec3 r2 = M.contacts[c] - other.mPosition;
        glm::mat4 i1 = InvTensor();
        glm::mat4 i2 = other.InvTensor();

        // Relative velocity
        vec3 relativeVel =
            (other.mVelocity + glm::cross(other.angVel, r2)) - (mVelocity + glm::cross(angVel, r1));
        // Relative collision normal
        vec3 relativeNorm = M.normal;
        relativeNorm = glm::normalize(relativeNorm);
        // Moving away from each other? Do nothing!
        if (glm::dot(relativeVel, relativeNorm) > 0.0f) {
            return;
        }

        // find  j: the magnitude of the impulse needed to resolve the collision.
        float e = fminf(cor, other.cor);
        float numerator = (-(1.0f + e) * glm::dot(relativeVel, relativeNorm));
        float d1 = invMassSum;
        vec3 d2 = glm::cross(Geometry::MultiplyVector(glm::cross(r1, relativeNorm), i1), r1);
        vec3 d3 = glm::cross(Geometry::MultiplyVector(glm::cross(r2, relativeNorm), i2), r2);
        float denominator = d1 + glm::dot(relativeNorm, d2 + d3);
        float j = (denominator == 0.0f) ? 0.0f : numerator / denominator;
        if (M.contacts.size() > 0.0f && j != 0.0f) {
            j /= (float)M.contacts.size();
        }

        // Apply Linear Impulse to the rigidbodies
        vec3 impulse = relativeNorm * j;
        mVelocity = mVelocity - impulse * invMass1;
        other.mVelocity = other.mVelocity + impulse * invMass2;
        angVel = angVel - Geometry::MultiplyVector(glm::cross(r1, impulse), i1);
        other.angVel = other.angVel + Geometry::MultiplyVector(glm::cross(r2, impulse), i2);

        // Friction
        vec3 t = relativeVel - (relativeNorm * glm::dot(relativeVel, relativeNorm));
        if (CMP(glm::dot(t, t), 0.0f)) {
            return;
        }
        t = glm::normalize(t);

        // find jt: the magnitude of the friction we are applying to this collision
        numerator = -glm::dot(relativeVel, t);
        d1 = invMassSum;
        d2 = glm::cross(Geometry::MultiplyVector(glm::cross(r1, t), i1), r1);
        d3 = glm::cross(Geometry::MultiplyVector(glm::cross(r2, t), i2), r2);
        denominator = d1 + glm::dot(t, d2 + d3);
        if (denominator == 0.0f) {
            return;
        }

        float jt = numerator / denominator;
        if (M.contacts.size() > 0 && jt != 0.0f) {
            jt /= (float)M.contacts.size();
        }
        if (CMP(jt, 0.0f)) {
            return;
        }

        // Property of friction called Coulomb's Law
        float Friction = sqrtf(friction * other.friction);
        if (jt > j * Friction) {
            jt = j * Friction;
        } else if (jt < -j * Friction) {
            jt = -j * Friction;
        }

        // Apply the tangential impulse (friction) to the velocity of each rigidbody
        vec3 tangentImpulse = t * jt;

        mVelocity = mVelocity - tangentImpulse * invMass1;
        other.mVelocity = other.mVelocity + tangentImpulse * invMass2;
        angVel = angVel - Geometry::MultiplyVector(glm::cross(r1, tangentImpulse), i1);
        other.angVel = other.angVel + Geometry::MultiplyVector(glm::cross(r2, tangentImpulse), i2);
    }
}

void Elys::RigidBody::AddLinearImpulse(const vec3 &impulse) {
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

void Elys::RigidBody::SyncCollisionVolumes(Node& node, const Mesh& mesh) {
    std::visit(
        overloaded{[&](AABB &aabb) { aabb.Update(node.InheritedTransform(), mesh); },
                   [&](OBB &obb) {
                       obb.Update(node.InheritedPosition(),
                                  glm::mat3_cast(node.InheritedRotation()));
                   }},
        mVolume);
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

    auto size = std::visit<glm::vec3>(
        overloaded{
            [](AABB const &aabb) { return glm::vec3{aabb.Size()}; },
            [](OBB const &obb) { return obb.Size(); }
        },
        mVolume
    );

    if (mass != 0) {
        size *= 2.0f;
        float fraction = (1.0f / 12.0f);
        float x2 = size.x * size.x;
        float y2 = size.y * size.y;
        float z2 = size.z * size.z;
        ix = (y2 + z2) * mass * fraction;
        iy = (x2 + z2) * mass * fraction;
        iz = (x2 + y2) * mass * fraction;
        iw = 1.0f;
    }

    return glm::inverse(glm::mat4(
        ix, 0, 0, 0,
        0, iy, 0, 0,
        0, 0, iz, 0,
        0, 0, 0, iw));

}
void Elys::RigidBody::AddRotationalImpulse(const vec3 &point, const vec3 &impulse) {
    vec3 centerOfMass = mPosition;
    vec3 torque = glm::cross(point - centerOfMass, impulse);

    vec3 angAccel = Geometry::MultiplyVector(torque, InvTensor());
    angVel = angVel + angAccel;
}
