//
// Created by Sam on 3/30/2022.
//

#ifndef ELYS_MESH_RENDERER_HPP
#define ELYS_MESH_RENDERER_HPP

#include <Render/Mesh.hpp>
#include <Render/Material.hpp>

namespace Elys {
    struct MeshRenderer {
        Mesh Mesh;
        Material Material;
    };
}

#endif // ELYS_MESH_RENDERER_HPP
