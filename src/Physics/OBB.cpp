//
// Created by pierrhum on 08/05/2022.
//

#include "Physics/OBB.hpp"

namespace Elys {
    OBB::OBB(glm::vec3 const &center, glm::vec3 const &s, glm::mat3 const &rotation) :
        mCenter(center), mSize(s), mRotation(rotation) {
        UpdateVertices();
    }

    void OBB::Update(glm::vec3 const &center, glm::mat3 const &rotation) {
        mCenter = center;
        mRotation = rotation;

        UpdateVertices();
    }

    void OBB::UpdateVertices() {
        mVertices = {
            mCenter + mRotation[0] * mSize[0] + mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] + mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] - mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] + mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] - mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter + mRotation[0] * mSize[0] - mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] + mRotation[1] * mSize[1] - mRotation[2] * mSize[2],
            mCenter - mRotation[0] * mSize[0] - mRotation[1] * mSize[1] + mRotation[2] * mSize[2],
        };
    }

    void OBB::UpdateBuffers() {
        // VERTEX ARRAY ALREADY EXIST THEN UPDATE ONLY VERTEX BUFFER DATA
        if (mVAO) {
            mVAO->GetVertexBuffer()->SetData((void *)mVertices.data(),
                                             static_cast<uint32_t>(mVertices.size() * sizeof(glm::vec3)),
                                             GL_DYNAMIC_DRAW);
        }
        // INIT VERTEX ARRAY
        else {
            // LINE INDICES
            static uint32_t indices[] = {
                6, 1,
                6, 3,
                6, 4,
                2, 7,
                2, 5,
                2, 0,
                0, 1,
                0, 3,
                7, 1,
                7, 4,
                4, 5,
                5, 3
            };

            mVAO = std::make_shared<VertexArray>();
            auto vbo = std::make_shared<VertexBuffer>(
                (void *)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(glm::vec3)),
                GL_DYNAMIC_DRAW);

            auto ebo = std::make_shared<IndexBuffer>(indices, 24);
            BufferLayout vertexLayout{{"position", sizeof(glm::vec3), 3, GL_FLOAT}};
            vbo->SetLayout(vertexLayout);
            mVAO->SetVertexBuffer(vbo);
            mVAO->SetIndexBuffer(ebo);
        }
    }

    std::pair<float, float> OBB::GetInterval(glm::vec3 const &axis) const {
        if (mVertices.empty()) return {0, 0};

        std::pair<float, float> result{
            glm::dot(axis, mVertices[0]),
            glm::dot(axis, mVertices[0])
        };

        for(uint8_t i = 1; i < 8; i++) {
            float projection = glm::dot(axis, mVertices[i]);

            result.first = projection < result.first ? projection : result.first;
            result.second = projection > result.second ? projection : result.second;
        }

        return result;
    }

    bool OBB::CollapseOnAxis(const OBB &left, const OBB &right, const glm::vec3 &axis) {
        auto a = left.GetInterval(axis);
        auto b = right.GetInterval(axis);

        return ((b.first <= a.second) && (a.first <= b.second));
    }

    bool OBB::Collapse(const OBB &left, const OBB &right) {
        std::array<glm::vec3, 15> testAxis = {
            left.mRotation[0], left.mRotation[1], left.mRotation[2],
            right.mRotation[0], right.mRotation[1], right.mRotation[2]
        };

        for(uint8_t i = 0; i < 3; i++) {
            testAxis[6 + i * 3 + 0] = glm::cross(testAxis[i], testAxis[3]);
            testAxis[6 + i * 3 + 1] = glm::cross(testAxis[i], testAxis[4]);
            testAxis[6 + i * 3 + 2] = glm::cross(testAxis[i], testAxis[5]);
        }

        for(auto const &axis : testAxis) {
            if (!CollapseOnAxis(left, right, axis)) return false;
        }

        return true;
    }
    bool OBB::PointInOBB(const glm::vec3 &point) const {
        glm::vec3 dir = point - mCenter;

        for (int i = 0; i < 3; ++i) {
            const float* orientation = glm::value_ptr(mRotation);
            glm::vec3 axis(
                orientation[0],
                orientation[1],
                orientation[2]);
            float distance = glm::dot(dir, axis);
            if (distance > mSize[i]) {
                return false;
            }
            if (distance < -mSize[i]) {
                return false;
            }
        }
        return true;
    }

    std::vector<Geometry::Line> OBB::GetEdges() const {
        std::vector<Geometry::Line> result;
        result.reserve(12);
        std::vector<glm::vec3> v = GetVertices();
        // Declare an array that holds pairs of indices into the vector of vertices.
        int index[][2] = { // Indices of edge-vertices
                          {6,1},{6,3},{6,4},{2,7},{2,5},{2,0},
                          {0,1},{0,3},{7,1},{7,4},{4,5},{5,3}
        };
        // Loop through the index array and construct edges from the vertex pairs
        for (int j = 0; j < 12; ++j) {
            result.push_back(Geometry::Line(
                v[index[j][0]], v[index[j][1]]
                ));
        }

        return result;
    }
    std::vector<Geometry::Plan> OBB::GetPlanes() const {
        glm::vec3 c = mCenter; // OBB Center
        glm::vec3 e = mSize; // OBB Extents
        const float* o = glm::value_ptr(mRotation);

        glm::vec3 a[] = { // OBB Axis
            glm::vec3(o[0], o[1], o[2]),
            glm::vec3(o[3], o[4], o[5]),
            glm::vec3(o[6], o[7], o[8]),
        };

        std::vector<Geometry::Plan> result;
        result.resize(6);

        result[0] = Geometry::Plan(a[0], glm::dot(a[0], (c + a[0] * e.x)));
        result[1] = Geometry::Plan(a[0]*-1.0f,-glm::dot(a[0],(c-a[0]*e.x)));
        result[2] = Geometry::Plan(a[1], glm::dot(a[1], (c + a[1] * e.y)));
        result[3] = Geometry::Plan(a[1]*-1.0f,-glm::dot(a[1],(c-a[1]*e.y)));
        result[4] = Geometry::Plan(a[2], glm::dot(a[2], (c + a[2] * e.z)));
        result[5] = Geometry::Plan(a[2]*-1.0f,-glm::dot(a[2],(c-a[2]*e.z)));

        return result;
    }

    std::vector<glm::vec3> OBB::ClipEdgesToOBB(const std::vector<Geometry::Line> &edges) const {
        std::vector<glm::vec3> result;
        result.reserve(edges.size());
        glm::vec3 intersection;
        const std::vector<Geometry::Plan>& planes = GetPlanes();
        for (int i = 0; i<planes.size(); ++i) {
            for (int j = 0; j <edges.size(); ++j) {
                if (ClipToPlane(planes[i],
                                edges[j], &intersection)) {
                    if (PointInOBB(intersection)) {
                        result.push_back(intersection);
                    }
                }
            }
        }
        return result;
    }
    float OBB::PenetrationDepth(const OBB &o1, const OBB &o2, const glm::vec3 &axis, bool *outShouldFlip) {
        std::pair<float, float> i1 = o1.GetInterval(glm::normalize(axis));
        std::pair<float, float> i2 = o2.GetInterval(glm::normalize(axis));

        // If the intervals do not overlap, there is no penetration
        if (!((i2.first <= i1.second) && (i1.first <= i2.second))) {
            return 0.0f; // No penetration
        }

        // Length of both intervals
        float len1 = i1.second - i1.first;
        float len2 = i2.second - i2.first;

        // Find the smallest and largest points out of both the intervals
        float min = fminf(i1.first, i2.first);
        float max = fmaxf(i1.second, i2.second);

        float length = max - min;

        if (outShouldFlip != 0) {
            *outShouldFlip = (i2.first < i1.first);
        }

        // Return the length of the intersection
        return (len1 + len2) - length;
    }

    Geometry::CollisionManifold OBB::FindCollisionFeatures(const OBB &A, const OBB &B) {
        Geometry::CollisionManifold result;
        ResetCollisionManifold(&result);

        const float* o1 = glm::value_ptr(A.GetRotation());
        const float* o2 = glm::value_ptr(B.GetRotation());

        // Construct a SAT test
        glm::vec3 test[15] = { // Face axis
                              glm::vec3(o1[0], o1[1], o1[2]),
                              glm::vec3(o1[3], o1[4], o1[5]),
                              glm::vec3(o1[6], o1[7], o1[8]),
                              glm::vec3(o2[0], o2[1], o2[2]),
                              glm::vec3(o2[3], o2[4], o2[5]),
                              glm::vec3(o2[6], o2[7], o2[8])
        };
        for (int i = 0; i< 3; ++i) { // Fill out rest of axis
            test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
            test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
            test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
        }

        glm::vec3* hitNormal = 0;
        bool shouldFlip;

        for (int i = 0; i< 15; ++i) {
            // Avoid the edge case of a malformed axis
            if (glm::dot(test[i], test[i])< 0.001f) {
                continue;
            }
            // Find the penetration depth of the OBBs
            float depth = PenetrationDepth(A, B,test[i], &shouldFlip);

            // No intersections
            if (depth <= 0.0f) {
                return result;
            } else if (depth <result.depth) {
                if (shouldFlip) {
                    test[i] = test[i] * -1.0f;
                }
                // Store the depth and collision normal
                result.depth = depth;
                hitNormal = &test[i];
            }
        }
        if (hitNormal == 0) {
            return result;
        }
        glm::vec3 axis = glm::normalize(*hitNormal);

        // Clip each oriented bounding box against the other
        std::vector<glm::vec3> c1 = A.ClipEdgesToOBB(B.GetEdges());
        std::vector<glm::vec3> c2 = B.ClipEdgesToOBB(A.GetEdges());
        result.contacts.reserve(c1.size() + c2.size());
        result.contacts.insert(result.contacts.end(),
                               c1.begin(), c1.end());
        result.contacts.insert(result.contacts.end(),
                               c2.begin(), c2.end());

        // Project the result of the clipped points onto a shared plane
        std::pair<float, float> i = A.GetInterval(axis);
        float distance = (i.second - i.first)* 0.5f - result.depth * 0.5f;
        glm::vec3 pointOnPlane = A.GetPosition() + axis * distance;
        for (int i = result.contacts.size() - 1; i>= 0; --i) {
            glm::vec3 contact = result.contacts[i];
            result.contacts[i] = contact + (axis * glm::dot(axis, pointOnPlane - contact));
        }
        result.colliding = true;
        result.normal = axis;
        return result;
    }

}
