//
// Created by pierrhum on 08/05/2022.
//

#ifndef ELYS_BOUNDING_BOX_HPP
#define ELYS_BOUNDING_BOX_HPP

#include <variant>

#include <glm/glm.hpp>

#include <Render/DataHolder.hpp>
#include <Physics/AABB.hpp>
#include <Physics/OBB.hpp>

namespace Elys {
    typedef std::variant<AABB, OBB> Volume;

    void UpdateBuffers(Volume &volume);
    void DrawVolume(Volume &volume);
    bool Intersect(Volume &left, Volume &right);
} // namespace Elys
#endif // ELYS_BOUNDING_BOX_HPP
