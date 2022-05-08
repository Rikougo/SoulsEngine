//
// Created by pierrhum on 08/05/2022.
//

#include "Render/OBB.hpp"
Elys::OBB::OBB(const Elys::Mesh &mesh) {
    vec3 lo = vec3(std::numeric_limits<float>::max());
    vec3 hi = vec3(std::numeric_limits<float>::min());

    for (Vertex v : mesh.Vertices()) {
        if (v.position.x < lo.x)
            lo.x = v.position.x;
        if (v.position.y < lo.y)
            lo.y = v.position.y;
        if (v.position.z < lo.z)
            lo.z = v.position.z;

        if (v.position.x > hi.x)
            hi.x = v.position.x;
        if (v.position.y > hi.y)
            hi.y = v.position.y;
        if (v.position.z > hi.z)
            hi.z = v.position.z;
    }
    center = vec3((hi.x+lo.x)/2, (hi.y+lo.y)/2, (hi.z+lo.z)/2);
    mForward = vec3((hi.x+lo.x)/2, 0, 0);
    mUp = vec3(0, (hi.y+lo.y)/2, 0);
    mRight = vec3(0, 0, (hi.z+lo.z)/2);

}
void Elys::OBB::Update(glm::mat4 transform, const Elys::Mesh &mesh) {
    if (transform == mTransform)
        return;
    // center = mesh.position
    // mForward,mUp,mRight rotate selon mesh.rotation
}

void Elys::OBB::UpdateVertices() {
    mVertices = {// FRONT
                 (center - mForward - mUp - mRight), (center + mForward - mUp - mRight),

                 (center + mForward - mUp - mRight), (center + mForward + mUp - mRight),

                 (center + mForward + mUp - mRight), (center - mForward + mUp - mRight),

                 (center - mForward + mUp - mRight), (center - mForward - mUp - mRight),

                 // SIDE
                 (center - mForward - mUp - mRight), (center - mForward - mUp + mRight),

                 (center + mForward - mUp - mRight), (center + mForward - mUp + mRight),

                 (center + mForward + mUp - mRight), (center + mForward + mUp + mRight),

                 (center - mForward + mUp - mRight), (center - mForward + mUp + mRight),

                 // BACK
                 (center + mForward - mUp + mRight), (center - mForward - mUp + mRight),

                 (center - mForward - mUp + mRight), (center - mForward + mUp + mRight),

                 (center - mForward + mUp + mRight), (center + mForward + mUp + mRight),

                 (center - mForward + mUp + mRight), (center + mForward - mUp + mRight)};
}
